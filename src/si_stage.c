#include "c_depot_list.h"

#include "t_log_handler.h"
#include "t_depot_manager.h"
#include "t_config_tool.h"

#include "e_map_manager.h"
#include "e_engine_settings.h"

#include "si_stage.h"
#include "si_save_manager.h"

struct Stage *si_init_stage(int map_index){
	struct Stage *stage = XCALLOC(1, sizeof(struct Stage));
	stage->map_manager = e_create_map_manager(map_index);	
	if(!stage->map_manager){LOG(LOG_NULL, "Failed to create map_manager"); free(stage); return NULL;}
	
	struct DepotManagerFile *depo_file = t_create_depotmanfile(DEPOT_COUNT);	
	
	for(int i = 0; i < DEPOT_COUNT; i++){
		depo_file->depot_item_cap[i] = e_grab_depoitemcount(i);
	}
	
	stage->depot_manager = t_create_depot_manager(depo_file);
	
	load_depot_DPO_ITEM_PROTO(stage->depot_manager);	
	load_depot_DPO_SPRITE(stage->depot_manager);
	load_depot_DPO_INTER_PROTO(stage->depot_manager);
	load_depot_DPO_ENTITY_PROTO(stage->depot_manager);

	stage->save_manager = si_create_save_manager();

	return stage;
}
void si_free_stage(struct Stage *stage){
	if(!stage){return;}
	si_free_save_manager(stage->save_manager);
	stage->save_manager = NULL;
	
	e_free_map_manager(stage->map_manager);
	stage->map_manager = NULL;

	t_free_depot_manager(stage->depot_manager);
	stage->depot_manager = NULL;

	free(stage);
	stage = NULL;
}
