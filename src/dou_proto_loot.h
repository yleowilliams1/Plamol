#pragma once
#include <stdbool.h>
#include "c_magic_number.h"

struct LootData{
	int items_gindx[LOOT_MAX_SIZE];
	bool is_random;
};

struct DouLoader;

struct DouLoader dou_loot();
