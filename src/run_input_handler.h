#pragma once
struct World;
struct InputManager;
typedef struct Camera2D;

void run_update_input_handler(struct World *w, struct InputManager *input, Camera2D *cam);
