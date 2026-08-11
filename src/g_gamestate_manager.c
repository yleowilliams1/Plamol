#include <raylib.h>
#include <stdio.h>
#include "g_gamestate_manager.h"
#include "e_engine_settings.h"
#include "e_error_handler.h"
#include "t_config_tool.h"
#include "t_strings.h"
#include "p_instantiator.h"
#include "p_entity_instance.h"
#include "i_input_handler.h"
#include "m_render_map.h"
#include "l_sprite_manager.h"

static void gamestate_parser(struct config_pack, void *ptr);

struct GameState gstate = {0};

Camera2D *grab_cam(){
	return &gstate.cam;
}

void g_load_gamestate(){
	char *path = e_grab_str(GAMESTATE_PATH);
	bool win = t_config(NULL, path, gamestate_parser); 
	if(!win){ERR_LOG(ERR_PARSE, "Failed to parse gamestate %s", path);}
	
	m_load_map(gstate.gindx_strtmap);
	gstate.map = m_get_map(gstate.gindx_strtmap, true);
	p_instantiate_entities(gstate.map);
	
	// Setup camera
	gstate.cam = (Camera2D){0};
	gstate.cam.zoom = 1.0f;
	gstate.cam.offset = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
	ERR_LOG(ERR_OK, "Succesfully loaded gamestate!");
}


void g_update_gamestate(){
	update_input_handler();	
}
void  g_draw_gamestate(){
	BeginDrawing();
		ClearBackground(BLACK);
		BeginMode2D(gstate.cam);
			m_draw_map(&gstate.map, gstate.player_z);
			e_draw_entity_pool(grab_entity_pool(), grab_entity_pool_size());
				
		EndMode2D();
	EndDrawing();
}

static void gamestate_parser(struct config_pack p, void *ptr){
	if(t_check(p.current_section, "Map")){
		if(t_check(p.key, "starting_map_gindx")){
			t_atoi(p.value, &gstate.gindx_strtmap);
		}	
	}
}
