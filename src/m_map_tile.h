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
	FLAG_COUNT,
};

enum TileInfo{
	T_PORTRAIT,
	T_TEXT,
	T_COMBAT,
	T_LOOT,
	T_COMBAT_LOOT,
	T_FLAGS,
	T_COUNT,
};
struct TileData{
	int data[T_COUNT];
};

bool t_free_tile(int gindx);
bool t_load_tile(int gindx);
int t_grab_tiledata(int gindx, enum TileInfo t, bool autoload);
