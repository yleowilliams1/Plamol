#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include "t_math.h"
#include "i_input.h"
#include "g_gamestate_manager.h"
#include "w_window_manager.h"

#define MIN_ZOOM 0.5f
#define MAX_ZOOM 6.0f
static void ih_handle_zoom_camera();
static void ih_handle_pan_camera();

static Camera2D *cam = NULL;

static Vector2 current_mouse;
static Vector2 last_mouse;
static float wheel;

static float base_zoom = 1.0f;

void update_input_handler(){
	if(cam == NULL){cam = grab_cam();}
	
	current_mouse = GetMousePosition();
	wheel = GetMouseWheelMove();

	ih_handle_zoom_camera();
	ih_handle_pan_camera();
	last_mouse = current_mouse;
}

static void ih_handle_zoom_camera(){

	if(wheel != 0.0f){
		Vector2 m_before = GetScreenToWorld2D(current_mouse, *cam);
		
		float old_zoom = cam->zoom;
		cam->zoom += wheel * 0.1f;
		
		float clamped_zoom = cam->zoom;
		if(clamped_zoom < MIN_ZOOM){clamped_zoom = MIN_ZOOM;}
		if(clamped_zoom > MAX_ZOOM){clamped_zoom = MAX_ZOOM;}


		if(clamped_zoom != old_zoom){
			cam->zoom = clamped_zoom;
			Vector2 m_after = GetScreenToWorld2D(current_mouse, *cam);
			cam->target.x += m_before.x - m_after.x;
			cam->target.y += m_before.y - m_after.y;
		} else {
			cam->zoom = clamped_zoom;
		}
	} 
}

static void ih_handle_pan_camera(){
    	Vector2 current_mouse = { (float)GetMouseX(), (float)GetMouseY() }; 
	if (!i_input_held(A_PAN)) {
        	last_mouse = current_mouse;
        	return;
    	}

    	Vector2 delta = {
        	current_mouse.x - last_mouse.x,
        	current_mouse.y - last_mouse.y,
    	};
    
	cam->target.x -= delta.x / cam->zoom;
	cam->target.y -= delta.y / cam->zoom;
}
