#pragma once

#include <raylib.h>

struct InputManager;
struct World;
struct DouManager;

struct RuntimeData{
	Camera2D cam;
};

struct RuntimeData *run_create_runtime();
void run_update_runtime(struct World *world, struct DouManager *protos, struct InputManager *input, float dt);
void run_draw_runtime(struct World *world);
void run_free_runtime(struct RuntimeData *run);
