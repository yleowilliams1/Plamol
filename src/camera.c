#include <raylib.h>
#include "util/util.h"
#include "settings.h"

void update_cam(Camera2D *cam){
	static float base_zoom = 1.0f;
	Vector2 cpos = GetMousePosition();

	// Scale the zoom when the window size changes
	if(IsWindowResized()){
		Vector2 world_pos_b = GetScreenToWorld2D(cpos, *cam);
		base_zoom = wincvrt(base_zoom, BY_LARGEST);
		cam->zoom = base_zoom;
		Vector2 world_pos_a = GetScreenToWorld2D(cpos, *cam);

		// Center
		cam->target.x += world_pos_b.x - world_pos_a.x;
		cam->target.y += world_pos_b.y - world_pos_a.y;
	}

	// Do the input here eventually
	if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)){
		Vector2 delta = GetMouseDelta();
		cam->target.x -= delta.x / cam->zoom;
		cam->target.y -= delta.y / cam->zoom;
	}

	float wheel = GetMouseWheelMove();
	if(wheel != 0){
		Vector2 world_pos_b = GetScreenToWorld2D(cpos, *cam);
		float factor = FLT(CAM_FACTOR);
		if(wheel > 0){base_zoom *= factor;}
		else{base_zoom /= factor;}

		// Clamp
		if(base_zoom < FLT(CAM_MIN_ZOOM)){base_zoom = FLT(CAM_MIN_ZOOM);}
		if(base_zoom > FLT(CAM_MAX_ZOOM)){base_zoom = FLT(CAM_MAX_ZOOM);}

		cam->zoom = base_zoom;
		Vector2 world_pos_a = GetScreenToWorld2D(cpos, *cam);
		// Center
		cam->target.x += world_pos_b.x - world_pos_a.x;
		cam->target.y += world_pos_b.y - world_pos_a.y;
	} else{cam->zoom = base_zoom;}
}
