#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "si_map.h"

#include "t_pool.h"
#include "t_math.h"

#include "c_magic_number.h"
#include "run_runtime.h"

struct DouManager;
struct InventoryManager;
struct World {
	struct MapPack *map;
	struct Pool entities;
	struct Pool interactables;
		    
	struct InRef *occupancy;
	
	struct InRef player;
	struct InRef kill_queue[MAX_KILLS_PER_FRAME];
	
	int kill_count;

	struct RuntimeData *runtime;
};

struct World *si_load_world(struct DouManager *protos, int map_gindx);
void si_free_world(struct World *w);
void si_update_world(struct World *w, struct DouManager *proto, float dt, struct InputManager *input);
void si_draw_world(struct World *world);
bool si_try_move(struct World *w, struct InRef ref, v3 to);
void si_queue_kill(struct World *w, struct InRef r);
