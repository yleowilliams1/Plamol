#include <stdio.h>
#include "t_log_handler.h"

#include "e_coordinator.h"
#include "e_engine_settings.h"

#include "si_flags.h"
#include "si_input.h"
#include "si_map.h"

#include "e_dou_manager.h"


struct Coordinator *initalize_game(){
	struct Coordinator *cor = XCALLOC(1, sizeof(struct Coordinator)); 
	e_load_engine_settings();
	cor->flag_manager = si_init_flag();
	if(cor->flag_manager == NULL){LOG(LOG_NULL, "Failed to load flag manager");}
	cor->input = si_init_input();
	if(cor->flag_manager == NULL){LOG(LOG_NULL, "Failed to load input manager");}

	cor->dous = e_create_dou_manager();
	if(cor->dous == NULL){LOG(LOG_NULL, "Failed to load dou-manager");}
	return cor;
}
void free_game(struct Coordinator *cor){
	e_free_dou_manager(cor->dous);
	if(cor->loaded_map){si_free_map(cor->loaded_map);}
	if(cor->flag_manager){si_free_flag(cor->flag_manager);}
	if(cor->input){free(cor->input);}
	e_free_settings();
}
