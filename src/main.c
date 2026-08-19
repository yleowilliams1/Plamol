#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include "w_window_manager.h"
#include "e_coordinator.h"
#define TARGET_FPS 60

int main(){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(w_base_res_x, w_base_res_y, "Game");
	SetTargetFPS(TARGET_FPS);

	srand(time(NULL));

	struct Coordinator *cor = e_initalize_game();
	
	while (!WindowShouldClose()){
		w_update_relative_scale();	
		e_update_game(cor);
		e_draw_game(cor);
	}
	
	e_free_game(cor);
	CloseWindow();
	return 0;
}
