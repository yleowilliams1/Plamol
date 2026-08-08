#include "s_game.h"
#include "g_statemachine.h"
#include "w_window_manager.h"

void game_enter(void){
}
void game_exit(void){
}
void game_update(void){
}
void game_render(void){
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

