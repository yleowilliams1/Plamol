#pragma once
#include <stdbool.h>
#include "t_math.h"
#include "si_map.h"

#define PLAYER_INSTANCE_INDEX 0

struct Occupant{
	v2 tile;
	bool valid;	
};

struct MapManager{
	struct MapPack *map_pack;
	struct Occupant *occupancy;
};

struct MapManager *e_create_map_manager(int map_index);
void e_free_map_manager(struct MapManager *map_manager);
