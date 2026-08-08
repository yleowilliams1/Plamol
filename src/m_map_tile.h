#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
enum Flags{
	TFLAG_LOOK,
	TFLAG_INTERACT,
	TFLAG_LOOTABLE,
	TFLAG_DIALOGUE,
	TFLAG_COMBAT,
	TFLAG_COUNT,
};

struct TileData{
	uint32_t flags;
};

bool t_free_tile(int gindx);
bool t_load_tile(int gindx);
int t_grab_tiledata(int gindx, bool autoload);
