#pragma once
#include <stdint.h>
#include <stdbool.h>

#define MAX_SEGS 256 
#define ENTITY_SIZE 256

enum MetadataProperties{
	M_NORTH_EXIT,
	M_SOUTH_EXIT,
	M_WEST_EXIT,
	M_EAST_EXIT,
	M_ENTITY_COUNT,
	M_MAP_MESH,
	M_MAP_GINDX,
	M_META_COUNT,
};

struct MapEntityData{
	int gindx;
	int spawn_x;
	int spawn_y;
	int GUID;
	bool valid;
};
struct MapData{
	int meta[M_META_COUNT];
	struct MapEntityData entities[ENTITY_SIZE];
};

bool m_free_map(int gindx);
bool m_load_map(int gindx);
struct MapData m_get_map(int gindx, bool autoload);
