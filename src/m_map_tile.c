#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include "m_map_tile.h"
#include "e_engine_settings.h"

#define MAX_TILES_LOADED 256

struct TileData *tile_data;

bool l_read_tile(uint8_t tileset, uint16_t tile_index){
	char *string = e_get_path_to_tilesets();
	size_t len = strlen(string);
	bool needs_slash = (len == 0 || string[len - 1] != '/');
	
	char *path = malloc(len + needs_slash + 5);
	if(!path){return false;}

	strcpy(path, string);
	if(needs_slash){path[len] = '/'; len++;}
		
	char *file = malloc(strlen(path) + 8);
	sprintf(file, "%s%u.ini", path, (unsigned int)tile_index);	
		
	// Load the ini file into a tile data
	char line[256];
	char current_section[64] = {0};
	FILE *f = fopen(file, "r");
	if(!f){return false;}
	
	if(tile_data){free(tile_data);}
	tile_data = calloc(0, sizeof(struct TileData));
	
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
				if(strcmp(key, "tile_texture_id") == 0){
					tile_data->tile_texture_index = atoi(v);
				}
				else if(strcmp(key, "description_index") == 0){
					tile_data->description_index = atoi(v); 
				}
				else if(strcmp(key, "combat_encounter_index") == 0){
					tile_data->combat_encounter_index = atoi(v);
				}
				else if(strcmp(key, "base_loottable_index") == 0){
					tile_data->base_loottable_index = atoi(v);	
				}
				else if(strcmp(key, "combat_loottable_index") == 0){
					tile_data->combat_loottable_index = atoi(v);
				}
			}
			else if(strcmp(current_section, "Flags") == 0){
				if(strcmp(key, "look") == 0 && atoi(v)){
					tile_data->flags |= (1 << FLAG_LOOK);
				}
				else if(strcmp(key, "interact") == 0 && atoi(v)){
					tile_data->flags |= (1 << FLAG_INTERACT);
				}
				else if(strcmp(key, "lootable") == 0 && atoi(v)){
					tile_data->flags |= (1 << FLAG_LOOTABLE);
				}
				else if(strcmp(key, "dialogue") == 0 && atoi(v)){
					tile_data->flags |= (1 << FLAG_DIALOGUE);
				}
				else if(strcmp(key, "combat") == 0 && atoi(v)){
					tile_data->flags |= (1 << FLAG_COMBAT);
				}
			}
		}
	}		
	
	fclose(f);
	free(path);
	free(file);
	return true;
}

void m_cleanup_tileset(){
	free(tile_data);
}

void m_load_tileset(uint8_t tileset, uint16_t tile_index){
		
}

