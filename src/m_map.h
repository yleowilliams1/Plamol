#pragma once
#include <stdint.h>
#include <stdbool.h>

struct MapTileData{
	int  tile_indx;
	bool is_passable;
	int  tile_texture_index;
	int  start_x;
	int  start_y;
	int  end_x;
	int  end_y;
};
struct MapData{
	int tileset;
	int count;
	struct MapTileData data[];
};

struct MetadataTemp{
	int tileset;
	int count;
};
void m_init(void);
void m_free();
