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
#include "l_asset_manager.h"

#define MAX_TILES 16 

static void tile_parser(struct config_pack p, void *ptr);

static struct TileData tiles[MAX_TILES] = {0};
static struct local_indx indx_man[MAX_TILES] = {0};

static const char *flag_lookup[TFLAG_COUNT] = {
	[TFLAG_LOOK] = "look",
	[TFLAG_INTERACT] = "interact",
	[TFLAG_LOOTABLE] = "lootable",
	[TFLAG_DIALOGUE] = "dialogue", 
	[TFLAG_COMBAT] = "combat", 
};

static const char *tiledata_lokup[TILEINFO_COUNT] = {
	[T_PORTRAIT] = "portrait",
	[T_TEXT]     = "text",
	[T_COMBAT]   = "combat",
	[T_LOOT]     = "loot",
	[T_COMBAT_LOOT] = "combat_loot",
};

bool t_free_tile(int gindx){

	struct AssetFreePackage pckg = {
		.gindx = gindx,
		.index_manager = indx_man,
		.arr_cap = MAX_TILES,
		.arr = tiles,
		.element_size = sizeof(struct TileData),
	};
	bool succed = t_free_asset(pckg); 
		
	return succed;
}

bool t_load_tile(int gindx){
	
	struct AssetLoadPackage pckg = {
		.gindx = gindx,
		.index_manager = indx_man,
		.arr_cap = MAX_TILES,
		.arr = tiles,
		.element_size = sizeof(struct TileData),
		.function = tile_parser,
		.path = e_grab_str(TILE_PATH),	
		.init = NULL,
	};

	bool success = l_load_asset(pckg);
	
	return success;	
}

int t_grab_tiledata(int gindx, enum TileInfo t, bool autoload){
	int lindx = l_getter_checks(gindx, autoload, MAX_TILES, indx_man, t_load_tile); 
	if(!t_indxvalid(MAX_TILES, lindx)){
		ERR_LOG(ERR_FUCKED, "Couldn't find or load %d", gindx);
	}
	
	return tiles[lindx].data[t];
}
static void tile_parser(struct config_pack p, void *ptr){
	struct TileData *tile = (struct TileData *)ptr;		
	if(!tile){ERR_LOG(ERR_FUCKED, "Took null pointer into parser, This sohuldn't be possible");}

	if(t_check(p.current_section, "DataIndexes")){
		for(int i = 0; i < TILEINFO_COUNT; i++){
			char *str = (char *)tiledata_lokup[i];
			// string cannot be null
			if(!t_check(p.key, str)){continue;}
			t_atoi(p.value, &tile->data[i]);
		}
	}
	if(t_check(p.current_section, "Flags")){
		for(int i = 0; i < TFLAG_COUNT; i++){
			char *str = (char *)flag_lookup[i];
			if(!t_check(p.key, str)){continue;}
			
			int value;
			t_atoi(p.value, &value);
			if(value < 0){ERR_LOG(ERR_PARSE, "Tried to parse flag %s with value of less than 0", str);}
			if(value > 0){tile->data[T_FLAGS]|= (1 << i);}
		}	
	}
}


