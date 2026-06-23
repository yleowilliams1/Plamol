#pragma once
#include <stdint.h>
#include <stdbool.h>
#pragma pack(push, 1)

// Map tiles are fixed in memory. The indentifer
// for the data is not.
struct MapTileData{
	uint16_t tile_indx;
	bool     is_passable;
};
struct MapData{
	uint8_t  tileset;
	uint32_t width;
	uint32_t height;
	struct MapTileData data[];
};
#pragma pack(pop)

bool l_write_map(const char *filepath, struct MapData *map);
struct MapData *l_read_map(const char *filepath);
