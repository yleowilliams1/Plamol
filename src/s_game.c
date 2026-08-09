#include "s_game.h"
#include "f_flags.h"
#include "e_engine_settings.h"
#include "i_input.h"
#include "g_statemachine.h"
#include "w_window_manager.h"
#include "g_gamestate_manager.h"
#include "e_error_handler.h"

void game_enter(void){
	e_load_engine_settings();
	f_init_flag();
	i_init_input();
	g_load_gamestate();
	ERR_LOG(ERR_OK, "Entered Game Succesfully");
}
void game_exit(void){
	e_free_setting();
	f_free_flag();
}
void game_update(void){
}
void game_render(void){
	g_draw_gamestate();
}
void game_pause(void){
}
void game_resume(void){
}
struct GameStateNode game_state(void){
	return (struct GameStateNode){
		.on_enter = game_enter,
		.on_exit = game_exit,
		.on_update = game_update,
		.on_render = game_render,
		.on_pause = game_pause,
		.on_resume = game_resume,
	};
}

