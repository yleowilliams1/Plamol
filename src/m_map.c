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

static void m_decompress_segments(struct MapRuntime *run, const struct MapMetadata *meta);

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
	[INVISIBLE] = "invisible",
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
	[HIDE_IF_ABOVE_PLAYER] = "hide_if_above_player",
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
	free(runtimemaps[lindx].t);
	if (runtimemaps[lindx].s) {
		free(runtimemaps[lindx].s);
		runtimemaps[lindx].s = NULL;
	}		
	if(runtimemaps[lindx].i){
		free(runtimemaps[lindx].i);
		runtimemaps[lindx].i = NULL;
	}	
	
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
	m->e = XMALLOC(g_loading_meta->meta[M_ENTITY_COUNT] * sizeof(struct MapEntityData));
	m->i = XMALLOC(g_loading_meta->meta[M_INTERACTABLE_COUNT] * sizeof(struct MapInteractableData));
	m->t = XMALLOC(g_loading_meta->meta[M_WIDTH] * g_loading_meta->meta[M_HEIGHT] * sizeof(struct MapDecompTile));
	m->s = XMALLOC(g_loading_meta->meta[M_SEGMENT_COUNT] * sizeof(struct MapSegmentData));

	memset(m->e, 0, g_loading_meta->meta[M_ENTITY_COUNT] * sizeof(struct MapEntityData));
	memset(m->i, 0, g_loading_meta->meta[M_INTERACTABLE_COUNT] * sizeof(struct MapInteractableData));
	memset(m->t, 0, g_loading_meta->meta[M_WIDTH] * g_loading_meta->meta[M_HEIGHT] * sizeof(struct MapDecompTile));
	memset(m->s, 0, g_loading_meta->meta[M_SEGMENT_COUNT] * sizeof(struct MapSegmentData));
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
		if(t_check(p.key, "z")){
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
	
	// Decompress segments & stamp interactables onto runtime grid
	m_decompress_segments(&map.d, &map.m);
	
	// Free segment data buffer after decompression
	if (runtimemaps[runtime_lindx].s) {
		free(runtimemaps[runtime_lindx].s);
		runtimemaps[runtime_lindx].s = NULL;
		map.d.s = NULL;
	}		
	if(runtimemaps[runtime_lindx].i){
		free(runtimemaps[runtime_lindx].i);
		runtimemaps[runtime_lindx].i = NULL;
		map.d.i = NULL;
	}	
	
	return map;
}
static void m_decompress_segments(struct MapRuntime *run, const struct MapMetadata *meta) {
	if (!run || !run->t || !meta) return;

	int width = meta->meta[M_WIDTH];
	int height = meta->meta[M_HEIGHT];
	int seg_count = meta->meta[M_SEGMENT_COUNT];
	int interactable_count = meta->meta[M_INTERACTABLE_COUNT];

	// 1. Process Segments into Decompressed Tiles
	if (run->s) {
	for (int i = 0; i < seg_count; i++) {
		struct MapSegmentData *seg = &run->s[i];

	    	int min_x = seg->start_tile.x < seg->end_tile.x ? seg->start_tile.x : seg->end_tile.x;
	    	int max_x = seg->start_tile.x > seg->end_tile.x ? seg->start_tile.x : seg->end_tile.x;
	    	int min_y = seg->start_tile.y < seg->end_tile.y ? seg->start_tile.y : seg->end_tile.y;
	    	int max_y = seg->start_tile.y > seg->end_tile.y ? seg->start_tile.y : seg->end_tile.y;

	    	if (min_x < 0) min_x = 0;
	    	if (min_y < 0) min_y = 0;
	    	if (max_x >= width)  max_x = width - 1;
	    	if (max_y >= height) max_y = height - 1;

	    	for (int y = min_y; y <= max_y; y++) {
			for (int x = min_x; x <= max_x; x++) {
		    		int tile_idx = y * width + x;
		    		struct MapDecompTile *tile = &run->t[tile_idx];
				
		    		bool is_edge_x = (x == min_x || x == max_x);
		    		bool is_edge_y = (y == min_y || y == max_y);
		    		bool is_perimeter = is_edge_x || is_edge_y;

		    		// Corner detection
		    		if (is_edge_x && is_edge_y) {
					tile->flags |= (1 << T_IS_CORNER);
		    		}

		    		// Wall Placement & Direction Resolution
		    		bool place_wall = false;
		    		enum WallDirections dir = W_NORTH;

		    		if ((seg->flags & (1 << HAS_WALLS_REC)) && is_perimeter) {
					place_wall = true;
					if (y == min_y) dir = W_NORTH;
					else if (y == max_y) dir = W_SOUTH;
					else if (x == min_x) dir = W_EAST;
					else if (x == max_x) dir = W_WEST;
		    		} else if (seg->flags & (1 << IS_WALLS)) {
					if ((seg->flags & (1 << WALL_IS_NORTH)) && y == min_y) { place_wall = true; dir = W_NORTH; }
					else if ((seg->flags & (1 << WALL_IS_SOUTH)) && y == max_y) { place_wall = true; dir = W_SOUTH; }
					else if ((seg->flags & (1 << WALL_IS_WEST)) && x == min_x) { place_wall = true; dir = W_WEST; }
					else if ((seg->flags & (1 << WALL_IS_EAST)) && x == max_x) { place_wall = true; dir = W_EAST; }
					else {
			    			uint32_t dir_mask = (1 << WALL_IS_NORTH) | (1 << WALL_IS_SOUTH) |(1 << WALL_IS_EAST)  | (1 << WALL_IS_WEST);
			    			if ((seg->flags & dir_mask) == 0) place_wall = true;
					}
		    		}

		    		if (place_wall) {
					tile->flags |= (1 << T_HAS_WALL);
					tile->wall_gindx = seg->wall_gindx;
					tile->wall_z = seg->z;
					tile->dir = dir;
		    		}

		    		// Floor Placement
		    		if (seg->flags & (1 << HAS_FLOORS_REC)) {
					tile->flags |= (1 << T_HAS_TILE);
					tile->floor_gindx = seg->floor_gindx;
					tile->floor_z = seg->z;
		    		}

		    		// Map Segment Flags to TileFlags
		    		if (seg->flags & (1 << INVISIBLE))           tile->flags |= (1 << T_INVISIBLE);
		    		if (seg->flags & (1 << IS_WALLS_COLLIDE))    tile->flags |= (1 << T_WALL_COLLIDE);
		    		if (seg->flags & (1 << IS_FLOOR_COLLIDE))    tile->flags |= (1 << T_FLOOR_COLLIDE);
		    		if (seg->flags & (1 << ALWAYS_ABOVE_PLAYER)) tile->flags |= (1 << T_ALWAYS_ABOVE);
		    		if (seg->flags & (1 << SHOULD_MERGE_WALL))   tile->flags |= (1 << T_MERGE_WALL);
		    if (seg->flags & (1 << SHOULD_MERGE_FLOOR))  tile->flags |= (1 << T_MERGE_FLOOR);
		}
	    }
	}
	}

	// 2. Stamp Interactables into Tile Grid
	if (run->i) {
	for (int i = 0; i < interactable_count; i++) {
	    struct MapInteractableData *inter = &run->i[i];

	    int x = inter->tile_position.x;
	    int y = inter->tile_position.y;

	    if (x >= 0 && x < width && y >= 0 && y < height) {
		int tile_idx = y * width + x;
		struct MapDecompTile *tile = &run->t[tile_idx];

		tile->flags |= (1 << T_HAS_INTERACTABLE);
		tile->interactable_gindx = inter->gindx;
		tile->interactable_z = inter->z;
	    }
	}
	}
}
