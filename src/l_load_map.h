#pragma once
#include <stdint.h>
#include <stdbool.h>
#pragma pack(push, 1)

// Map tiles are fixed in memory. The indentifer
// for the data is not.
struct MapTileData{
	// Rendering
	uint8_t tileset_indentifier;
	uint16_t tile_texture_indentifier;
	// Data
	uint32_t tile_data_indentifier;
	// 1+2+4 = 7
	
	uint8_t padding;

	// 8 total
};
struct MapData{
	uint32_t width;
	uint32_t height;
	struct MapTileData data[];
};
#pragma pack(pop)

bool l_write_map(const char *filepath, struct MapData *map);
struct MapData *l_read_map(const char *filepath);
