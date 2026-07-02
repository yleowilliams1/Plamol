#include <raylib.h>
#include "colors.h"
#include "g_statemachine.h"
#include "w_window_manager.h"
#include "s_combat.h"

void combat_enter(){
}
void combat_exit(){
}
void combat_update(){
}
void combat_render(){
}
void combat_pause(){
}
void combat_resume(){
}

struct GameStateNode combat_state(void){
	return (struct GameStateNode){
		.on_enter = combat_enter,
		.on_exit = combat_exit,
		.on_update = combat_update,
		.on_render = combat_render,
		.on_pause = combat_pause,
		.on_resume = combat_resume,	
	};
}
