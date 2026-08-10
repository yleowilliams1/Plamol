#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "m_map.h"
#include "t_config_tool.h"
#include "e_engine_settings.h"
#include "e_error_handler.h"
#include "t_strings.h"
#include "t_gindex_tool.h"
#include "l_asset_manager.h"

#define MAP_CAP 16

// This is SUPERRRR unoptimized 
// its fine for now but eventually I need to write a custom thing so that
// it's cleaner and faster

static void metadata_parser(struct config_pack p, void *ptr);
static void runtime_parser(struct config_pack p, void *ptr);

static enum InteractableType str_to_inttype(char *str);
static void runtime_init(void *ptr);


static struct MapMetadata metamaps[MAP_CAP] = {0};
static struct local_indx iman_meta[MAP_CAP] = {0};

static struct MapRuntime runtimemaps[MAP_CAP] = {0};
static struct local_indx iman_runtime[MAP_CAP] = {0};

static struct MapMetadata *g_loading_meta;

static const char *metadata_lokup[M_META_COUNT] = {
	[M_NORTH_EXIT] = "north_map",
	[M_SOUTH_EXIT] = "south_map",
	[M_WEST_EXIT] = "west_map",
	[M_EAST_EXIT] = "east_map",
	[M_ENTITY_COUNT] = "entity_count",
	[M_INTERACTABLE_COUNT] = "interactable_count",
	[M_SEGMENT_COUNT] = "segment_count",
	[M_WIDTH] = "width",
	[M_HEIGHT] = "height",
};
static const char *segment_flag_lokup[SEGMENT_FLAGS_COUNT] = {
	[IS_VISIBLE] = "is_visible",
	[IS_WALLS] = "is_wall",
	[WALL_IS_NORTH] = "wall_is_north",
	[WALL_IS_SOUTH] = "wall_is_south",
	[WALL_IS_EAST] = "wall_is_east",
	[WALL_IS_WEST] = "wall_is_west",
	[HAS_WALLS_REC] = "has_walls_rec",
	[HAS_FLOORS_REC] = "has_floors_rec",
	[IS_WALLS_COLLIDE] = "is_walls_collide",
	[IS_FLOOR_COLLIDE] = "is_floor_collide",
	[ALWAYS_ABOVE_PLAYER] = "always_above_player",
	[SHOULD_MERGE_WALL] = "should_merge_wall",
	[SHOULD_MERGE_FLOOR] = "should_merge_floor",
};

bool m_free_map(int gindx){
	// We can free the metadata with
	// the asset manager but the 
	// runtime data is malloced so can't be
	struct AssetFreePackage pckg = {
		.gindx = gindx,
		.index_manager = iman_meta,
		.arr_cap = MAP_CAP,
		.arr = metamaps,
		.element_size = sizeof(struct MapMetadata),
	};

	bool freed_meta = t_free_asset(pckg);

	int lindx = t_gindx_to_lindx(iman_runtime, MAP_CAP, gindx);
	if(!t_indxvalid(MAP_CAP, lindx)){ERR_LOG(ERR_INDX ,"Failed to convert gindx %d", gindx); return false;}	
	// Free here
	free(runtimemaps[lindx].e);	
	free(runtimemaps[lindx].i);	
	free(runtimemaps[lindx].s);

	runtimemaps[lindx] = (struct MapRuntime){0};	
	ERR_LOG(ERR_OK, "Freed map %d", gindx);	
	return t_lfree_lindx(iman_runtime, MAP_CAP, lindx) && freed_meta;
}

bool m_load_map(int gindx){
	
	// metadata can go through
	// the asset loader pipline but
	// due to the malloc the runtime data
	// can't	
	struct AssetLoadPackage meta_pckg = {
		.gindx = gindx,
		.index_manager = iman_meta,
		.arr_cap = MAP_CAP,
		.arr = metamaps,
		.element_size = sizeof(struct MapMetadata),
		.function = metadata_parser,
		.path = e_grab_str(MAP_METADATA_PATH),
		.init = NULL,
	};	
	struct AssetLoadPackage runtime_pckg = {
		.gindx = gindx,
		.index_manager = iman_runtime,
		.arr_cap = MAP_CAP,
		.arr = runtimemaps,
		.element_size = sizeof(struct MapRuntime),
		.function = runtime_parser,
		.path = e_grab_str(MAP_PATH),
		.init = runtime_init,
	};
	
	bool meta_loaded = l_load_asset(meta_pckg);
	if(!meta_loaded){ERR_LOG(ERR_FUCKED, "Failed to load map metadata"); return false;}
	int meta_lindx = t_gindx_to_lindx(iman_meta, MAP_CAP, gindx);
	if(!t_indxvalid(MAP_CAP, meta_lindx)){ERR_LOG(ERR_FUCKED, "Metadata lindx invalid after load"); return false;}
	g_loading_meta = &metamaps[meta_lindx];
	bool runtime_loaded = l_load_asset(runtime_pckg);
	g_loading_meta = NULL;
	ERR_LOG(ERR_OK, "Loaded map %d", gindx);
	return meta_loaded && runtime_loaded;
}

static void runtime_init(void *ptr){
	struct MapRuntime *m = (struct MapRuntime *)ptr;
	if(!m || !g_loading_meta){ERR_LOG(ERR_FUCKED, "Failed to load map asset passed null pointer to init");}
	m->e = malloc(g_loading_meta->meta[M_ENTITY_COUNT] * sizeof(struct MapEntityData));
	m->i = malloc(g_loading_meta->meta[M_INTERACTABLE_COUNT] * sizeof(struct MapInteractableData));
	m->s = malloc(g_loading_meta->meta[M_SEGMENT_COUNT] * sizeof(struct MapSegmentData));
}
static void metadata_parser(struct config_pack p, void *ptr){
	struct MapMetadata *m = (struct MapMetadata*)ptr;	
	if(!m){
		ERR_LOG(ERR_FUCKED, "Passed null metadata to parser. Shouldn't be possible");	
	}
	
	if(t_check(p.current_section, "metadata")){
		for(int i = 0; i < M_META_COUNT; i++){
			if(!t_check(p.key, (char *)metadata_lokup[i])){continue;}
			t_atoi(p.value, &m->meta[i]);	
		}
	}
}
static void runtime_parser(struct config_pack p, void *ptr){
	struct MapRuntime *m = (struct MapRuntime*)ptr;	
	if(!m || !g_loading_meta){ERR_LOG(ERR_FUCKED, "Missing meta context on runtime parse"); return;}	
	
	int entity_indx;
	if(sscanf(p.current_section, "entity.%d", &entity_indx) == 1){
		if(entity_indx < 0){ERR_LOG(ERR_PARSE, "Tried to create parse entity with a negative index"); return;}	
		if(entity_indx >= g_loading_meta->meta[M_ENTITY_COUNT]){ERR_LOG(ERR_PARSE, "Tried to parse entity with index larger than set entity count %d", g_loading_meta->meta[M_ENTITY_COUNT]); return;}
		if(t_check(p.key, "entity_gindx")){
			t_atoi(p.value, &m->e[entity_indx].gindx);
		} else if(t_check(p.key, "world_spawn_x")){
			t_atoi(p.value, &m->e[entity_indx].world_spawn_x);
		} else if(t_check(p.key, "world_spawn_y")){
			t_atoi(p.value, &m->e[entity_indx].world_spawn_y);
		} else if(t_check(p.key, "GUID")){
			t_atoi(p.value, &m->e[entity_indx].GUID);
		} 
	}
	int interactable_indx;
	if(sscanf(p.current_section, "interactable.%d", &interactable_indx) == 1){
		if(interactable_indx < 0){ERR_LOG(ERR_PARSE, "Tried to parse interactable with a negative index"); return;}	
		if(interactable_indx >= g_loading_meta->meta[M_INTERACTABLE_COUNT]){ERR_LOG(ERR_PARSE, "Tried to parse interactable with index larger than set interactable count %d", g_loading_meta->meta[M_INTERACTABLE_COUNT]); return;}
		if(t_check(p.key, "type")){
			enum InteractableType type = str_to_inttype(p.value);
			if(type == NULL_INTERACTABLE){ERR_LOG(ERR_PARSE, "%s is not a valid type", p.value); return;}
			m->i[interactable_indx].type = type;
		} else if(t_check(p.key, "z")){
			t_atoi(p.value, &m->i[interactable_indx].z);
		} else if(t_check(p.key, "tile_x")){
			t_atoi(p.value, &m->i[interactable_indx].tile_position.x);
		} else if(t_check(p.key, "tile_y")){
			t_atoi(p.value, &m->i[interactable_indx].tile_position.y);
		} else if(t_check(p.key, "interactable_gindx")){
			t_atoi(p.value, &m->i[interactable_indx].gindx);
		}	
	}
	int seg_indx;
	if(sscanf(p.current_section, "segment.%d", &seg_indx) == 1){
		if(seg_indx < 0){ERR_LOG(ERR_PARSE, "Tried to parse segment with a negative index"); return;}	
		if(seg_indx >= g_loading_meta->meta[M_SEGMENT_COUNT]){ERR_LOG(ERR_PARSE, "Tried to parse segment with index larger than set segment count %d", g_loading_meta->meta[M_SEGMENT_COUNT]); return;}
		for(int i = 0; i < SEGMENT_FLAGS_COUNT; i++){
			char *str = (char *)segment_flag_lokup[i];
			if(!t_check(p.key, str)){continue;}
			int value;
			t_atoi(p.value, &value);
			if(value > 0){m->s[seg_indx].flags |= (1 << i);}				
		}
		
		if(t_check(p.key, "start_tile_x")){
			t_atoi(p.value, &m->s[seg_indx].start_tile.x);
		} else if(t_check(p.key, "start_tile_y")){
			t_atoi(p.value, &m->s[seg_indx].start_tile.y);
		} else if(t_check(p.key, "end_tile_x")){
			t_atoi(p.value, &m->s[seg_indx].end_tile.x);
		} else if(t_check(p.key, "end_tile_y")){
			t_atoi(p.value, &m->s[seg_indx].end_tile.y);
		} else if(t_check(p.key, "z")){
			t_atoi(p.value, &m->s[seg_indx].z);
		} else if(t_check(p.key, "wall_gindx")){
			t_atoi(p.value, &m->s[seg_indx].wall_gindx);
		} else if(t_check(p.key, "floor_gindx")){
			t_atoi(p.value, &m->s[seg_indx].floor_gindx);
		}
	}
}

struct MapPack m_get_map(int gindx, bool autoload){
	int metadata_lindx = l_getter_checks(gindx, autoload, MAP_CAP, iman_meta, m_load_map);
	int runtime_lindx = l_getter_checks(gindx, autoload, MAP_CAP, iman_runtime, m_load_map);
	
	if(!t_indxvalid(MAP_CAP, metadata_lindx)){ERR_LOG(ERR_FUCKED, "Failed to find metadata for map %d", gindx);}
	if(!t_indxvalid(MAP_CAP, runtime_lindx)){ERR_LOG(ERR_FUCKED, "Failed to find runtime for map %d", gindx);}
	if(metadata_lindx != runtime_lindx){ERR_LOG(ERR_FUCKED, "Map %d desynced between metadata and runtime", gindx);}
	
	struct MapPack map = {
		.m = metamaps[metadata_lindx],
		.d = runtimemaps[runtime_lindx],
	};
	return map;
}
static enum InteractableType str_to_inttype(char *str){	
	if(t_check(str, "is_door")){return IS_DOOR;}
	if(t_check(str, "is_window")){return IS_WINDOW;}
	if(t_check(str, "is_trap")){return IS_TRAP;}
	if(t_check(str, "is_puzzel")){return IS_PUZZEL;}

	return NULL_INTERACTABLE;
}
