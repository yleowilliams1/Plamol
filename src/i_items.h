#pragma once
#include <stdbool.h>
#include <stdint.h>
enum ItemFlags{
	FLAG_THROWABLE,
	FLAG_CONSUMEABLE,
	FLAG_COUNT,
};

// If there are more than 255 stats I will commit a crime
struct ItemDataSet{
	int stat;
	int amount;
};

struct Item{
	struct ItemDataSet add;
	struct ItemDataSet use_hit;
	struct ItemDataSet use_damage;
	struct ItemDataSet use_consume;
	uint8_t flags;
	int tile_range;
	char *name;
	char *description;	
	bool valid;
};

struct BitFlagDef{
	char *string;
	uint8_t bit;
};

bool i_load_item(int gindx);
bool i_free_item(int gindx);
