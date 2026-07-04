#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "l_load_map.h"
#include "t_config_tool.h"
#define MAX_STRUCTS 512
#define MAX_FUNCTIONS 512

struct MapTileData g_pallete[MAX_STRUCTS] = {0};

bool l_write_map(const char *filepath, struct MapData *map){
	FILE *fp = fopen(filepath, "wb");
	if(!fp){return false;}	

	size_t size = sizeof(struct MapData) + map->width * map->height * sizeof(struct MapTileData);

	if(fwrite(map, size, 1, fp) != 1){fclose(fp); return false;}
	fclose(fp);
	return true;
}

void parse_map(struct config_pack p, void *ptr){
	struct MapData *m = (struct MapData*)ptr;	
	// Scan palletes
	int struct_indx;
	if(sscanf(p.current_section, "palletes.%d", &struct_indx) == 1){
		if(struct_indx < 0 || struct_indx >= MAX_STRUCTS){return;}
		struct MapTileData *slot = &g_pallete[struct_indx];	
		if(t_check(p.key, "tile_indx")){
			slot->tile_indx = (uint16_t)atoi(p.value);
		} else if(t_check(p.key, "tile_texture_index")){
			slot->tile_texture_index = atoi(p.value);
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
			m->data[func_indx].start_x = atoi(p.value);
		} else if(t_check(p.key, "start_y")){
			m->data[func_indx].start_y = atoi(p.value);
		} else if(t_check(p.key, "end_x")){
			m->data[func_indx].end_x = atoi(p.value);
		} else if(t_check(p.key, "end_y")){
			m->data[func_indx].end_y = atoi(p.value);
		} else if(t_check(p.key, "pallete")){
			int pallete = atoi(p.value);
			m->data[func_indx].tile_indx = g_pallete[pallete].tile_indx;
			m->data[func_indx].tile_texture_index = g_pallete[pallete].tile_texture_index;		
		} else if(t_check(p.key, "passable")){
			int passable = atoi(p.value);
			if(passable > 0){passable = 1;}
			if(passable < 0){passable = 0;}
			m->data[func_indx].is_passable = passable;
		}
		return;
	}	
}
void parse_meta(struct config_pack p, void *ptr){
	struct MetadataTemp *m = (struct MetadataTemp*)ptr;
	if(t_check(p.current_section, "metadata")){
		if(t_check(p.key, "count")){
			m->count = atoi(p.value);
		} else if(t_check(p.key, "tileset")){
			m->tileset = atoi(p.value);
		}
		return;
	}
}
struct MapData *l_read_map(const char *filepath){
	struct MapData *map; 
	struct MetadataTemp *meta = malloc(sizeof(struct MetadataTemp));
	
	// Get meta data first	
	t_config(meta, (char*)filepath, parse_meta);
	size_t size = sizeof(struct MapData) + meta->count * sizeof(struct MapTileData);
	map = calloc(1, size);
	map->count = meta->count;
	map->tileset = meta->tileset;
	
	t_config(map, (char *)filepath, parse_map);
	
	free(meta);
	return map;
}

