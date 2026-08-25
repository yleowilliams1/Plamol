#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>

#include "e_engine_state.h"
#include "w_window_manager.h"

#define TARGET_FPS 60

int main(){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(w_base_res_x, w_base_res_y, "Game");
	SetTargetFPS(TARGET_FPS);

	srand(time(NULL));

	struct EngineState *engine_state = e_create_engine_state();
	
	while (!WindowShouldClose()){
		w_update_relative_scale();	
		e_update_engine_state(engine_state);
		e_draw_engine_state(engine_state);
	}
	
	e_free_engine_state(engine_state);
	CloseWindow();
	return 0;
}
