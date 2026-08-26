#pragma once
#include <stdbool.h>
#include "t_math.h"
#include "si_map.h"

struct MapManager{
	struct Map *map_pack;
	// 2d heap array index with tile
	bool *occupancy;
};

struct MapManager *e_create_map_manager(int map_index);
void e_free_map_manager(struct MapManager *map_manager);
void e_occupy(struct MapManager *man, v2 tile);
void e_tclear(struct MapManager *man, v2 tile);
