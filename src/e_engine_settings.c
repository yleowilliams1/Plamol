#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e_engine_settings.h"
#include "i_input.h"
#include "t_config_tool.h"
#include "e_error_handler.h"
#include "t_strings.h"
#define INI_PATH "data/engine.ini"
#define MAX_BINDS 64

static struct EngineSettings *settings;

const char *str_lokup[ENG_STR_COUNT] = {
	[MAP_PATH] = "map_path",
	[TILESET_PATH] = "tileset_path",
	[TILE_PATH] = "tile_path",
	[PORTRAIT_PATH] = "portrait_path",
	[FLAG_PATH] = "flag_path",
	[ITEMS_PATH] = "items_path",
	[STATS_PATH] = "stats_path",
	[INVENTORY_PATH] = "inventory_path",
	[INPUT_PATH] = "input_path",
};

char *e_grab_str(enum EngStrings type){
	if(type >= ENG_STR_COUNT || type < 0){
		ERR_LOG(ERR_OUTOFBOUNDS, "Tried to access out of bounds string at index %d.", (int)type);		
		ERR_LOG(ERR_NULL, "Returning NULL to engine settings string request due to out of bounds request at %d.", (int)type);
		return NULL;
	}
	if(!settings){
		ERR_LOG(ERR_FUCKED, "Tried to access NULL settings.");
		return NULL;
	}
	char *str = settings->strings[type];
	if(!str){
		ERR_LOG(ERR_NULL, "Tried to access engine strings while engine strings aren't loaded.");
		ERR_LOG(ERR_FUCKED, "String lookup failed due to index %d not being present in the array. You fucked up here. Fix this shit. If your a player seeing this your entitled to a refund and front row seats to the execution of the  idiot that let this slide thorugh." , (int)type);
				return NULL;
	}

	return str;
}

void e_free_setting(){
	if(!settings){
		ERR_LOG(ERR_NULL, "Tried to double free settings");
		return;		
	}	

	for(int i = 0; i < ENG_STR_COUNT; i++){
		if(!settings->strings[i]){continue;}
		free(settings->strings[i]);
		settings->strings[i] = NULL;
	}
	free(settings);
	settings = NULL;
}
static void engine_parser(struct config_pack p, void *ptr){
	struct EngineSettings *s = (struct EngineSettings *)ptr;
	if(!s){
		ERR_LOG(ERR_FUCKED, "took null pointer in parser, should not be possible. If you see this, you fucked up big time.");	
		return;
	}
	if(t_check(p.current_section, "general")){
		for(int i = 0; i < ENG_STR_COUNT; i++){
			char *str = (char *)str_lokup[i];
			if(!str){
				ERR_LOG(ERR_FUCKED, "String lookup failed due to index %d not being present in the array. You fucked up here. Fix this shit. If your a player seeing this your entitled to a refund and front row seats to the execution of the  idiot that let this slide thorugh." ,i);
				continue;
			}
			if(t_check(p.key, str)){
				h_cpy(&s->strings[i], p.value);	
			}			
		}	
	}
}
bool e_load_engine_settings(){
	if(settings){
		ERR_LOG(ERR_RELOAD, "Attempted to reload engine settings while already allocated");
		e_free_setting();
	}

	settings = XCALLOC(1, sizeof(struct EngineSettings));

	bool conf_win = t_config((void *)settings, INI_PATH, engine_parser); 

	// Most likely if the config failes then its caught
	// before this return to main
	// but if it isn't main will need to crash to catch it.
	return conf_win;
}
