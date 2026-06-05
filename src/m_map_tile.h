#pragma once
#include <stdint.h>
#include <limits.h>
enum Flags{
    	FLAG_LOOK,
	FLAG_INTERACT,
	FLAG_LOOTABLE,
	FLAG_DIALOGUE,
	FLAG_COMBAT,
};

struct TileData{
	int tile_texture_index;
	int description_index;	
	int combat_encounter_index;
	int base_loottable_index;
	int combat_loottable_index;
	uint16_t flags;
};

struct Tileset{
	struct TileData tiles[USHRT_MAX];
};	
