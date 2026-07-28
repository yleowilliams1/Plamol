#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include "w_window_manager.h"
#include "e_engine_settings.h"
#include "f_flags.h"
#include "g_statemachine.h"
#include "s_menu.h"
#include "e_error_handler.h"
#define TARGET_FPS 60

int main(){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(w_base_res_x, w_base_res_y, "Game");
	SetTargetFPS(TARGET_FPS);

	srand(time(NULL));

	// Setup everything
	bool parsed = e_load_engine_settings();
		if(!parsed){ERR_LOG(ERR_FUCKED, "Engine prase failed!");}
	f_init_flag();
	sm_init(menu_state());
	
	while (!WindowShouldClose()){
		w_update_relative_scale();	
		sm_update();
		
		BeginDrawing();
			ClearBackground(DARKGRAY);
			sm_draw();
		EndDrawing();
	}

	e_free_setting();
	f_free_flag();
	CloseWindow();
	return 0;
}
