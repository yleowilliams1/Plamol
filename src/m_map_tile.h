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

enum TileInfo{
	T_PORTRAIT,
	T_TEXT,
	T_COMBAT,
	T_LOOT,
	T_COMBAT_LOOT,
	T_FLAGS,
	TILEINFO_COUNT,
};
struct TileData{
	int data[TILEINFO_COUNT];
};

bool t_free_tile(int gindx);
bool t_load_tile(int gindx);
int t_grab_tiledata(int gindx, enum TileInfo t, bool autoload);
