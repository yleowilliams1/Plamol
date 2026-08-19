#include <stdio.h>
#include "t_log_handler.h"

#include "e_coordinator.h"
#include "e_engine_settings.h"

#include "si_flags.h"
#include "si_input.h"
#include "si_world.h"

#include "e_dou_manager.h"

struct Coordinator *e_initalize_game(){
	struct Coordinator *cor = XCALLOC(1, sizeof(struct Coordinator)); 
	
	// Don't return if it returns null. If it passes through it will either
	// fix itself or abort descriptivly later before it damages anything
	
	e_load_engine_settings();
	cor->flag_manager = si_init_flag();
	if(cor->flag_manager == NULL){LOG(LOG_NULL, "Failed to load flag manager");}
	cor->input = si_init_input();
	if(cor->flag_manager == NULL){LOG(LOG_NULL, "Failed to load input manager");}

	cor->dous = e_create_dou_manager();
	if(cor->dous == NULL){LOG(LOG_NULL, "Failed to load dou-manager");}
	
	// This needs to be filled with a game loop system passing a map
	int map_gindx = 0;	
	cor->world = si_load_world(cor->dous, map_gindx);
	if(cor->world == NULL){LOG(LOG_NULL, "Failed to create map of gindx %d", map_gindx);}
	
	return cor;
}
void e_update_game(struct Coordinator *cor){
	float delta_time = GetFrameTime();;
	si_update_world(cor->world, cor->dous, delta_time, cor->input); 
}
void e_draw_game(struct Coordinator *cor){
	si_draw_world(cor->world);
}
void e_free_game(struct Coordinator *cor){
	e_free_dou_manager(cor->dous);
	if(cor->flag_manager){si_free_flag(cor->flag_manager);}
	if(cor->input){free(cor->input);}
	si_free_world(cor->world);
	e_free_settings();
}
