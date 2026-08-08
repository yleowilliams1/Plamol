#pragma once
#include <stdbool.h>

#define LOOT_SIZE 64

struct LootData{
	int items_gindx[LOOT_SIZE];
	bool is_random;
};

bool p_free_loot(int gindx);
bool p_load_loot(int gindx);
struct LootData p_grab_loot(int gindx, bool autoload);
