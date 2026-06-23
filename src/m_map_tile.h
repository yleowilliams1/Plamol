#pragma once
#include <stdbool.h>
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
	int tile_portrait_index;
	int text_index;	
	int combat_encounter_index;
	int base_loottable_index;
	int combat_loottable_index;
	uint16_t flags;
};

bool l_load_tile(uint8_t tileset, uint16_t tile_index);
struct TileData *grab_tile();
void m_cleanup_tile();
