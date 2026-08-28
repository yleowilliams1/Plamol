#pragma once
#include "e_map_manager.h"

struct Stage{
	struct MapManager *map_manager;		
};

struct Stage *si_init_stage(int map_index);
void si_free_stage(struct Stage *stage);
void si_draw_stage(struct Stage *stage);
