#include <stdio.h>
#include "g_gamestate_manager.h"
#include "e_engine_settings.h"
#include "e_error_handler.h"
#include "t_config_tool.h"
#include "t_strings.h"

static void gamestate_parser(struct config_pack, void *ptr);

struct GameState gstate = {0};

void g_load_gamestate(){
	char *path = e_grab_str(GAMESTATE_PATH);
	bool win = t_config(NULL, path, gamestate_parser); 
	if(!win){ERR_LOG(ERR_PARSE, "Failed to parse gamestate %s", path);}
}

void g_

static void gamestate_parser(struct config_pack p, void *ptr){
	if(t_check(p.current_section, "Map")){
		if(t_check(p.key, "starting_map_gindx")){
			t_atoi(p.value, &gstate.gindx_strtmap);
		}	
	}
}
