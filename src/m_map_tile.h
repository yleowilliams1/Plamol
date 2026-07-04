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
	int tile_portrait_index;
	int text_index;		
	int combat_encounter_index;
	int base_loottable_index;
	int combat_loottable_index;
	uint16_t flags;
};

struct TileMetadata{
	uint8_t tileset;
	uint16_t tile_index;
	float time;
};

struct Tile{
	struct TileData data;
	struct TileMetadata metadata;
};

void t_update_tiles();
void t_cleanup_tile();

// Getters return field values by copy — never a pointer into the tile
// pool — so callers are safe even if the tile is evicted/freed later.
// Integer getters return -1 if the tile isn't loaded/loadable.
int m_get_tile_portrait_index(uint8_t tileset, uint16_t tile_index);
int m_get_text_index(uint8_t tileset, uint16_t tile_index);
int m_get_combat_encounter_index(uint8_t tileset, uint16_t tile_index);
int m_get_base_loottable_index(uint8_t tileset, uint16_t tile_index);
int m_get_combat_loottable_index(uint8_t tileset, uint16_t tile_index);
uint16_t m_get_tile_flags(uint8_t tileset, uint16_t tile_index);
bool m_tile_has_flag(uint8_t tileset, uint16_t tile_index, enum Flags flag);
