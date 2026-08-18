#include <raylib.h>
#include "colors.h"
#include "g_statemachine.h"
#include "s_menu.h"
#include "w_window_manager.h"

void menu_enter(){
}
void menu_exit(){
}
void menu_update(){
}
void menu_render(){
	DrawRectangle(0,0,GetScreenWidth(), GetScreenHeight(), PL_LIGHT_BROWN);
	DrawText("Background Texture Here",wcnf(10.f),wcnf(10.f), wcnf(20.f), BLACK);
}
void menu_pause(){
}
void menu_resume(){
}

struct GameStateNode menu_state(void){
	return (struct GameStateNode){
		.on_enter = menu_enter,
		.on_exit = menu_exit,
		.on_update = menu_update,
		.on_render = menu_render,
		.on_pause = menu_pause,
		.on_resume = menu_resume,
	};
}
