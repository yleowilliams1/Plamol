#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <raylib.h>
#include "m_map_tile.h"
#include "e_engine_settings.h"
#include "t_config_tool.h"

/* Right now it doesn't differentiate between render data and
 * data-data. So eventually we need to seperate those from each other.
 * Render needs it's own loading style relative to the camera while 
 * data-data doesn't.*/

#define SLOTS 16 

struct Tile *tiles[SLOTS];

int l_load_tile(uint8_t tileset, uint16_t tile_index);

// Copies a tile's data out by value so callers never hold a pointer into
// the tile pool (which can be freed/evicted on any later frame).
// Returns false if the tile isn't loaded/loadable.
static bool get_tile_data_copy(uint8_t tileset, uint16_t tile_index, struct TileData *out){
	int index = -1;
	for(int i = 0; i < SLOTS; i++){
		if(!tiles[i]){continue;}
		struct TileMetadata mdata = tiles[i]->metadata;
		if(mdata.tileset != tileset || mdata.tile_index != tile_index){continue;}
		index = i;
		break;
	}
	// Tile isn't loaded
	if(index == -1){
		index = l_load_tile(tileset, tile_index);
		if(index == -1){
			return false;
		}
	}

	// Reset timer
	struct Tile *tile = tiles[index];
	tile->metadata.time = 0.0f;

	*out = tile->data;
	return true;
}

int m_get_tile_portrait_index(uint8_t tileset, uint16_t tile_index){
	struct TileData data;
	if(!get_tile_data_copy(tileset, tile_index, &data)){
		return -1;
	}
	return data.tile_portrait_index;
}

int m_get_text_index(uint8_t tileset, uint16_t tile_index){
	struct TileData data;
	if(!get_tile_data_copy(tileset, tile_index, &data)){
		return -1;
	}
	return data.text_index;
}

int m_get_combat_encounter_index(uint8_t tileset, uint16_t tile_index){
	struct TileData data;
	if(!get_tile_data_copy(tileset, tile_index, &data)){
		return -1;
	}
	return data.combat_encounter_index;
}

int m_get_base_loottable_index(uint8_t tileset, uint16_t tile_index){
	struct TileData data;
	if(!get_tile_data_copy(tileset, tile_index, &data)){
		return -1;
	}
	return data.base_loottable_index;
}

int m_get_combat_loottable_index(uint8_t tileset, uint16_t tile_index){
	struct TileData data;
	if(!get_tile_data_copy(tileset, tile_index, &data)){
		return -1;
	}
	return data.combat_loottable_index;
}

// Returns the raw flag bitfield. Prefer m_tile_has_flag() below for
// checking a single flag.
uint16_t m_get_tile_flags(uint8_t tileset, uint16_t tile_index){
	struct TileData data;
	if(!get_tile_data_copy(tileset, tile_index, &data)){
		return 0;
	}
	return data.flags;
}

bool m_tile_has_flag(uint8_t tileset, uint16_t tile_index, enum Flags flag){
	struct TileData data;
	if(!get_tile_data_copy(tileset, tile_index, &data)){
		return false;
	}
	return (data.flags & (1 << flag)) != 0;
}

void t_cleanup_tile(){
	for(int i = 0; i < SLOTS; i++){
		if(tiles[i]){
			free(tiles[i]);
		}
	}
}
int h_find_arr_indx(){
	int oldest_index = -1;
	float oldest_time = 0;
	for(int i = 0; i < SLOTS; i++){
		if(!tiles[i]){
			return i;
		}
		if(oldest_index == -1 || tiles[i]->metadata.time > oldest_time){
			oldest_time = tiles[i]->metadata.time;
			oldest_index = i;
		}		
	}
	free(tiles[oldest_index]);
	tiles[oldest_index] = NULL;
	return oldest_index;
}
void t_update_tiles(){
	for(int i = 0; i < SLOTS; i++){
		if(tiles[i]){
			tiles[i]->metadata.time += GetFrameTime();
			if(tiles[i]->metadata.time >= e_get_tilemem_secs()){
				free(tiles[i]);
				tiles[i] = NULL;
			}		
		}
	}		
}
int l_load_tile(uint8_t tileset, uint16_t tile_index){
	char *string = e_get_path_to_tilesets();
	size_t len = strlen(string);
	bool needs_slash = (len == 0 || string[len - 1] != '/');
	
	char *path = malloc(len + needs_slash + 5);
	if(!path){return -1;}

	strcpy(path, string);
	if(needs_slash){path[len] = '/'; len++;}
			
	char *file = malloc(strlen(path) + 8);
	if(!file){ free(path); return -1; }
	sprintf(file, "%s%u.ini", path, (unsigned int)tile_index);		
			
	// Load the ini file into a tile data
	char line[256];
	char current_section[64] = {0};
	FILE *f = fopen(file, "r");
	if(!f){free(path); free(file);return -1;}
					
	int arr_indx = h_find_arr_indx();
	struct Tile *tile = tiles[arr_indx];
	tile->data = (struct TileData){0};

	tile->metadata = (struct TileMetadata){0};		
	tile->metadata.tileset = tileset;
	tile->metadata.tile_index = tile_index;

	while(fgets(line, sizeof(line), f)){
		if(line[0] == '\n' || line[0] == '#' || line[0] == ';'){continue;}
		if(line[0] == '['){
			sscanf(line, "[%63[^]]]", current_section);
			continue;
		}		

		char key[64];
		char value[128];

		if(sscanf(line, "%63[^=]=%127[^\n]", key, value) == 2){
			char *v = value;
			while(*v == ' ' || *v == '\t') v++;
			if(strcmp(current_section, "Indexes") == 0){
				if(strcmp(key, "tile_portrait_index") == 0){
					tile->data.tile_portrait_index = atoi(v);
				}
				else if(strcmp(key, "text_index") == 0){
					tile->data.text_index = atoi(v); 
				}
				else if(strcmp(key, "combat_encounter_index") == 0){
					tile->data.combat_encounter_index = atoi(v);
				}
				else if(strcmp(key, "base_loottable_index") == 0){
					tile->data.base_loottable_index = atoi(v);		
				}
				else if(strcmp(key, "combat_loottable_index") == 0){
					tile->data.combat_loottable_index = atoi(v);
				}
			}
			else if(strcmp(current_section, "Flags") == 0){
				if(strcmp(key, "look") == 0 && atoi(v)){
					tile->data.flags |= (1 << FLAG_LOOK);
				}
				else if(strcmp(key, "interact") == 0 && atoi(v)){
					tile->data.flags |= (1 << FLAG_INTERACT);
				}
				else if(strcmp(key, "lootable") == 0 && atoi(v)){
					tile->data.flags |= (1 << FLAG_LOOTABLE);
				}
				else if(strcmp(key, "dialogue") == 0 && atoi(v)){
					tile->data.flags |= (1 << FLAG_DIALOGUE);
				}
				else if(strcmp(key, "combat") == 0 && atoi(v)){
					tile->data.flags |= (1 << FLAG_COMBAT);
				}
			}
		}
	}				
	
	fclose(f);
	free(path);
	free(file);
	return arr_indx;
}


