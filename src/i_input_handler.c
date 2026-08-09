#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include "t_math.h"
#include "i_input.h"
#include "g_gamestate_manager.h"

static void ih_handle_zoom_camera();
static void ih_handle_pan_camera();

void update_input_handler(){
	ih_handle_zoom_camera();
	ih_handle_pan_camera();
}
static float target_fovy = 50.0f;

static void ih_handle_zoom_camera(){
    float wheel = GetMouseWheelMove();
    if(wheel != 0){
        float zoom_step = 0.8f;
        target_fovy -= (wheel * zoom_step);
        if(target_fovy < 1.0f){target_fovy = 1.0f;}
        if(target_fovy > 200.0f){target_fovy = 200.0f;}
    }

    Camera *cam = grab_cam();
    cam->fovy += (target_fovy - cam->fovy) * 10.0f * GetFrameTime();
}

static Vector2 last_mouse = {0};
static void ih_handle_pan_camera(){
    // Use raw Raylib mouse coordinates to avoid double-scaling
    Vector2 current_mouse = { (float)GetMouseX(), (float)GetMouseY() }; 
    if (!i_input_held(A_PAN)) {
        last_mouse = current_mouse;
        return;
    }

    Vector2 delta = {
        current_mouse.x - last_mouse.x,
        current_mouse.y - last_mouse.y,
    };
    last_mouse = current_mouse;
    
    Camera *cam = grab_cam();        
    vf3 cam_target = {cam->target.x, cam->target.y, cam->target.z};
    vf3 cam_pos = {cam->position.x, cam->position.y, cam->position.z};
    vf3 cam_up = {cam->up.x, cam->up.y, cam->up.z};

    vf3 right;
    vf3 up; 
    
    vf3 forward = cam_target;
    vf3_min(&forward, cam_pos);
    vf3_nor(&forward);
    
    right = vf3_cross(forward, cam_up);
    vf3_nor(&right);
    
    up = vf3_cross(right, forward);
    vf3_nor(&up);   
    
    // ORTHOGRAPHIC SCALE:
    // fovy is total height in world units. Divide by screen pixel height to get units/pixel.
    float units_per_pixel = cam->fovy / (float)GetScreenHeight();

    vf3_mul(&right, vf3_mke(-delta.x * units_per_pixel));
    
    float fuckyou_bandadge = 2.0f;
    vf3_mul(&up, vf3_mke((delta.y * units_per_pixel)*fuckyou_bandadge));
    
    vf3 move = right;
    vf3_add(&move, up);
    
    cam->position.x += move.x;
    cam->position.y += move.y;
    cam->position.z += move.z;

    cam->target.x += move.x;
    cam->target.y += move.y;
    cam->target.z += move.z;
}
