#include <raylib.h>

#include "c_depot_list.h"
#include "c_instance_list.h"

#include "t_log_handler.h"
#include "t_depot_manager.h"
#include "t_instance_manager.h"
#include "t_config_tool.h"

#include "e_map_manager.h"
#include "e_engine_settings.h"

#include "si_stage.h"
#include "si_save_manager.h"
#include "si_map.h"
static void update_cam(Camera2D *cam);
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

	struct InstanceFunctions entity_fncs = entity_instance();
	struct InstanceFunctions interactable_fncs = instance_interactable();

	stage->entity_instances = t_create_instance_manager(entity_fncs, e_grab_instance_count(INST_ENTITY), sizeof(struct EntityPrototype));
	stage->interactable_instances = t_create_instance_manager(interactable_fncs, e_grab_instance_count(INST_INTERACTABLE), sizeof(struct InteractablePrototype));

	si_load_stage_instances(stage->save_manager, map_index, stage->depot_manager,
		stage->entity_instances,
		stage->map_manager->map_pack->entity_instances,
		stage->map_manager->map_pack->metadata[M_ENTITY_INSTANCE_COUNT],
		DPO_ENTITY_PROTO,
		stage->interactable_instances,
		stage->map_manager->map_pack->interactable_instances,
		stage->map_manager->map_pack->metadata[M_INTERACTABLE_INSTANCE_COUNT],
		DPO_INTER_PROTO);

	stage->camera.target = (Vector2){0, 0};
	stage->camera.offset = (Vector2){0, 0};
	stage->camera.rotation = 0.0f;
	stage->camera.zoom = 1.0f;
	
	return stage;
}
void si_free_stage(struct Stage *stage){
	if(!stage){return;}

	t_free_instance_manager(stage->entity_instances);
	stage->entity_instances = NULL;

	t_free_instance_manager(stage->interactable_instances);
	stage->interactable_instances = NULL;

	si_free_save_manager(stage->save_manager);
	stage->save_manager = NULL;
	
	e_free_map_manager(stage->map_manager);
	stage->map_manager = NULL;

	t_free_depot_manager(stage->depot_manager);
	stage->depot_manager = NULL;

	free(stage);
	stage = NULL;
}
void si_draw_stage(struct Stage *stage){
	update_cam(&stage->camera);
	BeginMode2D(stage->camera);
	
	EndMode2D();
}
void update_cam(struct Camera2D *cam){
	static float base_zoom = 1.0f;
	Vector2 c_mpos = GetMousePosition();
	if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)){
		Vector2 delta = GetMouseDelta();
		cam->target.x -= delta.x / cam->zoom;
		cam->target.y -= delta.y / cam->zoom;
	}

	float wheel = GetMouseWheelMove();
	if(wheel != 0){
		Vector2 w_before = GetScreenToWorld2D(c_mpos, *cam);
		float zoom_factor = 1.1f;
		if(wheel > 0){base_zoom *= zoom_factor;}
		else{base_zoom /= zoom_factor;}

		if(base_zoom < 0.5f){base_zoom = 0.5f;}
		if(base_zoom > 6.0f){base_zoom = 6.0f;}

		cam->zoom = base_zoom;
		Vector2 w_after = GetScreenToWorld2D(c_mpos, *cam);

		cam->target.x += w_before.x - w_after.x;
		cam->target.y += w_before.y - w_after.y;
	}else{cam->zoom = base_zoom;}
}
