#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <raylib.h>
#include "t_gindex_tool.h"
#include "m_map_tile.h"
#include "e_engine_settings.h"
#include "t_config_tool.h"
#include "e_error_handler.h"
#include "t_strings.h"

#define MAX_TILES 16 

void tile_parser(struct config_pack p, void *ptr);
static bool l_load_tile(int gindx);


static struct TileData tiles[MAX_TILES] = {0};
static struct local_indx indx_man[MAX_TILES] = {0};

static const char *flag_lookup[FLAG_COUNT] = {
	[FLAG_LOOK] = "look",
	[FLAG_INTERACT] = "interact",
	[FLAG_LOOTABLE] = "lootable",
	[FLAG_DIALOGUE] = "dialogue", 
	[FLAG_COMBAT] = "combat", 
};

static const char *tiledata_lokup[T_COUNT] = {
	[T_PORTRAIT] = "portrait",
	[T_TEXT]     = "text",
	[T_COMBAT]   = "combat",
	[T_LOOT]     = "loot",
	[T_COMBAT_LOOT] = "combat_loot",
};

int t_grab_tiledata(){
	
}
int t_grab_tileflag(){
}

static bool l_load_tile(int gindx){
	
	int lindx = t_gindx_to_lindx(indx_man, MAX_TILES, gindx);	
	if(lindx == NULL_INDX || lindx < 0){
		ERR_LOG(ERR_FUCKED, "lindx find failed");
	}
	// Zero out
	tiles[lindx] = (struct TileData){0};

	bool loaded = t_loader(gindx, indx_man, tile_parser, e_grab_str(TILE_PATH), &tiles[lindx], lindx); 	
	bool set    = t_lset_lindx(indx_man, MAX_TILES, gindx, lindx);
	
	// Leave error checking for getter function
	return (loaded && set);	
}

void tile_parser(struct config_pack p, void *ptr){
	struct TileData *tile = (struct TileData *)ptr;		
	if(!tile){ERR_LOG(ERR_FUCKED, "Took null pointer into parser, This sohuldn't be possible");}

	if(t_check(p.current_section, "DataIndexes")){
		for(int i = 0; i < T_COUNT; i++){
			char *str = (char *)tiledata_lokup[i];
			// string cannot be null
			if(t_check(p.key, str)){
				t_atoi(p.value, &tile->indexes[i]);
			}
		}
	}

	if(t_check(p.current_section, "Flags")){
		for(int i = 0; i < T_COUNT; i++){
			char *str = (char *)flag_lookup[i];
			if(t_check(p.key, str)){
				int value;
				t_atoi(p.value, &value);
				if(value < 0){ERR_LOG(ERR_PARSE, "Tried to parse flag %s with value of less than 0", str);}
				if(value > 0){tile->tile_flags |= (1 << i);}
			}		
		}	
	}
}


