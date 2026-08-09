#include <raylib.h>
#include "e_error_handler.h"
#include "t_config_tool.h"
#include "e_engine_settings.h"
#include "l_model_manager.h"

struct LoadedMapModel loaded_map = {0};

void l_load_map_model(int gindx){
	char *path = e_grab_str(MAP_MESHES);
	char *combined_path = t_glb_plus_indx(path, gindx);
	UnloadModel(loaded_map.model);
	loaded_map.model = LoadModel(combined_path);
	loaded_map.bounds = GetMeshBoundingBox(loaded_map.model.meshes[0]);
	ERR_LOG(ERR_OK, "Loaded map model %d at path %s", gindx, combined_path);
}

void l_draw_map_model(Camera cam){
	BeginMode3D(cam);
	DrawModel(loaded_map.model, (Vector3){0, 0, 0}, 1.0f, WHITE);
	EndMode3D();
}


