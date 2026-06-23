#pragma once
#include <stdbool.h>
#include <stdint.h>

enum ItemFlags{
	FLAG_THROWABLE,
	FLAG_CONSUMEABLE,
};

// If there are more than 255 stats I will commit a crime
struct ItemDataSet{
	uint8_t stat;
	int8_t amount;
};

struct Item{
	struct ItemDataSet add;
	struct ItemDataSet use_hit;
	struct ItemDataSet use_damage;
	struct ItemDataSet use_consume;
	uint8_t flags;
	uint16_t tile_range;
	char *name;
	char *description;	
	bool valid;
};

const struct Item *i_grab_item(int indx);
void i_load_item(int indx);
