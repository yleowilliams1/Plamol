#include <raylib.h>
#include <stdio.h>
#include "l_model_manager.h"
#include "g_gamestate_manager.h"
#include "e_engine_settings.h"
#include "e_error_handler.h"
#include "t_config_tool.h"
#include "t_strings.h"
#include "p_instantiator.h"
#include "p_entity_instance.h"

static void gamestate_parser(struct config_pack, void *ptr);

struct GameState gstate = {0};

Camera *grab_cam(){
	return &gstate.cam;
}

void g_load_gamestate(){
	char *path = e_grab_str(GAMESTATE_PATH);
	bool win = t_config(NULL, path, gamestate_parser); 
	if(!win){ERR_LOG(ERR_PARSE, "Failed to parse gamestate %s", path);}
	
	m_load_map(gstate.gindx_strtmap);
	gstate.map = m_get_map(gstate.gindx_strtmap, true);
	l_load_map_model(gstate.map.meta[M_MAP_MESH]);
	gstate.cam = (Camera){ { 0.0f, 10.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 50.0f, CAMERA_ORTHOGRAPHIC};	
	p_instantiate_entities(gstate.map);
	ERR_LOG(ERR_OK, "Succesfully loaded gamestate!");
}

void  g_draw_gamestate(){
	BeginDrawing();
		ClearBackground(BLACK);
		BeginMode3D(gstate.cam);
			l_draw_map_model();
			e_draw_entity_pool(grab_entity_pool(), grab_entity_pool_size());
		EndMode3D();
	EndDrawing();
}

static void gamestate_parser(struct config_pack p, void *ptr){
	if(t_check(p.current_section, "Map")){
		if(t_check(p.key, "starting_map_gindx")){
			t_atoi(p.value, &gstate.gindx_strtmap);
		}	
	}
}
