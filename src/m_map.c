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
#define MAX_STRUCTS 512
#define MAX_FUNCTIONS 512

// Maps are immutable

static void map_parser(struct config_pack p, void *ptr);
static void map_init(void *ptr);

static struct MapData maps[MAP_CAP] = {0};
static struct local_indx iman[MAP_CAP] = {0};


static const char *metadata_lokup[M_META_COUNT] = {
	[M_NORTH_EXIT] = "north_map",
	[M_SOUTH_EXIT] = "south_map",
	[M_WEST_EXIT] = "west_map",
	[M_EAST_EXIT] = "east_map",
	[M_ENTITY_COUNT] = "entity_count",
	[M_MAP_MESH] = "map_mesh_gindx",
	[M_MAP_GINDX] = "map_gindx",
};

bool m_free_map(int gindx){
	struct AssetFreePackage pckg = {
		.gindx = gindx,
		.index_manager = iman,
		.arr_cap = MAP_CAP,
		.arr = maps,
		.element_size = sizeof(struct MapData),		
	};
	
	ERR_LOG(ERR_OK, "Freed map %d", gindx);
	return t_free_asset(pckg);
}

bool m_load_map(int gindx){
	struct AssetLoadPackage pckg = {
		.gindx = gindx,
		.index_manager = iman,
		.arr_cap = MAP_CAP,
		.arr = maps,
		.element_size = sizeof(struct MapData),
		.function = map_parser,
		.path = e_grab_str(MAP_PATH),
		.init = map_init,
	};
	
	ERR_LOG(ERR_OK, "Loaded map %d", gindx);
	return l_load_asset(pckg);
}
static void map_init(void *ptr){
	struct MapData *m = (struct MapData *)ptr;
	for(int i = 0; i < ENTITY_SIZE; i++){
		m->entities[i] = (struct MapEntityData){0};
	}
}
struct MapData m_get_map(int gindx, bool autoload){
	struct MapData map = {0};
	int lindx = l_getter_checks(gindx, autoload, MAP_CAP, iman, m_load_map);
	if(!t_indxvalid(MAP_CAP, lindx)){ERR_LOG(ERR_NULL, "Couldn't find or load gindx %d", gindx); return map;}
	map = maps[lindx];
	return map;
}
static void map_parser(struct config_pack p, void *ptr){
	
	struct MapData *m = (struct MapData*)ptr;	
	if(!m){
		ERR_LOG(ERR_FUCKED, "Passed null map to parser. Shouldn't be possible");	
	}
	
	if(t_check(p.current_section, "metadata")){
		for(int i = 0; i < M_META_COUNT; i++){
			if(!t_check(p.key, (char *)metadata_lokup[i])){continue;}
			t_atoi(p.value, &m->meta[i]);	
		}
	}

	int entity_indx;
	if(sscanf(p.current_section, "entities.%d", &entity_indx) == 1){
		if(entity_indx < 0 || entity_indx >= ENTITY_SIZE){return;}
		if(t_check(p.key, "gindx")){
			t_atoi(p.value, &m->entities->gindx);
		} else if(t_check(p.key, "spawn_x")){
			t_atoi(p.value, &m->entities->spawn_x);	
		} else if(t_check(p.key, "spawn_y")){
			t_atoi(p.value, &m->entities->spawn_y);
		} else if(t_check(p.key, "empty")){
			int val;
			t_atoi(p.value, &val);
			if(val <= 0){m->entities->valid = true;}
		} else if(t_check(p.key, "GUID")){
			t_atoi(p.value, &m->entities->GUID);
		}
		
		return;
	}
}
