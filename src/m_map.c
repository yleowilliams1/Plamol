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

#define MAX_STRUCTS 512
#define MAX_FUNCTIONS 512

struct MapTileData g_pallete[MAX_STRUCTS] = {0};
static struct MapData *map_ptr = NULL;

void parse_map(struct config_pack p, void *ptr){
	
	struct MapData *m = (struct MapData*)ptr;	
	if(!m){
		ERR_LOG(ERR_FUCKED, "Passed null map to parser. Shouldn't be possible");		
	}
	// Scan palletes
	int struct_indx;
	// You should probably rewrite 
	// t_check to take a integer pointer so i don't
	// have this ugly thing.
	if(sscanf(p.current_section, "palletes.%d", &struct_indx) == 1){
		if(struct_indx < 0 || struct_indx >= MAX_STRUCTS){return;}
		struct MapTileData *slot = &g_pallete[struct_indx];	
		if(t_check(p.key, "tile_indx")){
			t_atoi(p.value, &slot->tile_indx);
		} else if(t_check(p.key, "tile_texture_index")){
			t_atoi(p.value, &slot->tile_texture_index);
		}
		return;	
	}
	// This will access older pallete memory
	// if you access a unset pallet. Thats an issue.
	// Too bad!
	int func_indx;
	if(sscanf(p.current_section, "function.%d", &func_indx) == 1){
		if(func_indx < 0 || func_indx >= MAX_FUNCTIONS){return;}
		if(t_check(p.key, "start_x")){
			t_atoi(p.value, &m->data[func_indx].start_x);
		} else if(t_check(p.key, "start_y")){
			t_atoi(p.value, &m->data[func_indx].start_y);
		} else if(t_check(p.key, "end_x")){
			t_atoi(p.value, &m->data[func_indx].end_x);
		} else if(t_check(p.key, "end_y")){
			t_atoi(p.value, &m->data[func_indx].end_y);
		} else if(t_check(p.key, "pallete")){
			int pallete; 
			t_atoi(p.value, &pallete);
			m->data[func_indx].tile_indx = g_pallete[pallete].tile_indx;
			m->data[func_indx].tile_texture_index = g_pallete[pallete].tile_texture_index;		
		} else if(t_check(p.key, "passable")){
			int passable; 
			t_atoi(p.value, &passable);
			if(passable > 0){passable = 1;}
			if(passable < 0){passable = 0;}
			m->data[func_indx].is_passable = passable;
		}
		return;
	}	
}
void parse_meta(struct config_pack p, void *ptr){
	struct MetadataTemp *m = (struct MetadataTemp*)ptr;
	if(!m){ERR_LOG(ERR_FUCKED, "Passed null pointer this sucks. Your fucked up here!");}
	if(t_check(p.current_section, "metadata")){
		if(t_check(p.key, "count")){
			t_atoi(p.value, &m->count);
		} else if(t_check(p.key, "tileset")){
			t_atoi(p.value, &m->tileset);
		}
		return;
	}
}
void m_init(void){
	char *filepath = e_grab_str(MAP_PATH);
	if(!filepath){ERR_LOG(ERR_FUCKED, "e_grab_str return NULL for MAP_PATH enum");}
	struct MetadataTemp *meta = XMALLOC(sizeof(struct MetadataTemp));
	
	// Get meta data first	
	bool win = t_config(meta, filepath, parse_meta);
	if(!win){ERR_LOG(ERR_FUCKED, "t_config failed to parse metadata for map");}
	size_t size = sizeof(struct MapData) + meta->count * sizeof(struct MapTileData);
	
	// Override the current path
	m_free();	

	map_ptr = XCALLOC(1, size);
	map_ptr->count = meta->count;
	map_ptr->tileset = meta->tileset;
	
	bool win_2 = t_config(map_ptr, (char *)filepath, parse_map);
	if(!win_2){ERR_LOG(ERR_FUCKED, "t_config failed to parse map");}	
	free(meta);
}
void m_free(){
	if(map_ptr){
		free(map_ptr);
		return;
	}
	ERR_LOG(ERR_NULL, "Tried to double free map");
}
