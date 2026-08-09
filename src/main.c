#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include "w_window_manager.h"
#include "e_engine_settings.h"
#include "f_flags.h"
#include "g_statemachine.h"
#include "s_game.h"
#include "e_error_handler.h"
#include "g_gamestate_manager.h"

#define TARGET_FPS 60

int main(){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(w_base_res_x, w_base_res_y, "Game");
	SetTargetFPS(TARGET_FPS);

	srand(time(NULL));
	
	printf("Up %d \n", KEY_W);
	printf("Down %d\n", KEY_S);
	printf("Left %d\n", KEY_A);
	printf("Right %d\n", KEY_D);
	printf("Action %d\n", KEY_E);
	printf("Pan %d\n", MOUSE_BUTTON_MIDDLE);

	//t_initalize_tests();
	sm_init(game_state());
	while (!WindowShouldClose()){
		w_update_relative_scale();	
		sm_update();
		sm_draw();
	}

	e_free_setting();
	f_free_flag();
	sm_free();
	CloseWindow();
	return 0;
}
