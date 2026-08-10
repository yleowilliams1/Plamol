#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include "t_math.h"
#include "i_input.h"
#include "g_gamestate_manager.h"
#include "w_window_manager.h"

static void ih_handle_zoom_camera();
static void ih_handle_pan_camera();

void update_input_handler(){
	ih_handle_zoom_camera();
	ih_handle_pan_camera();
}

static void ih_handle_zoom_camera(){
	static float base_zoom = 1.0f;

	Vector2 current_mouse = GetMousePosition();	
	Camera2D *cam = grab_cam();

	float wheel = GetMouseWheelMove();
	if(wheel != 0){
		Vector2 m_before = GetScreenToWorld2D(current_mouse, *cam);
		
		float factor = 1.1f;
		if(wheel > 0){
			base_zoom *= factor;
		} else {
			base_zoom /= factor;
		}

		if(base_zoom < 0.5f) base_zoom = 0.5f;
		if(base_zoom > 6.0f) base_zoom = 6.0f;

		cam->zoom = wcnf(cam->zoom);
		Vector2 m_after = GetScreenToWorld2D(current_mouse, *cam);
		
		cam->target.x += m_before.x - m_after.x;
		cam->target.y += m_before.y - m_after.y;	
	} else{
		cam->zoom = wcnf(cam->zoom);

	}
}

static void ih_handle_pan_camera(){
    	Vector2 current_mouse = { (float)GetMouseX(), (float)GetMouseY() }; 
    	static Vector2 last_mouse = {0};
	if (!i_input_held(A_PAN)) {
        	last_mouse = current_mouse;
        	return;
    	}

    	Vector2 delta = {
        	current_mouse.x - last_mouse.x,
        	current_mouse.y - last_mouse.y,
    	};
    	last_mouse = current_mouse;
    
    	Camera2D *cam = grab_cam();        
	cam->target.x -= delta.x / cam->zoom;
	cam->target.y -= delta.y / cam->zoom;
	
	last_mouse = current_mouse;

}
