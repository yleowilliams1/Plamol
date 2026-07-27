#pragma once
#include <stdbool.h>
#include "p_stats.h"

#define HOTBAR_SIZE 8
#define INVENTORY_SIZE 32

enum Stats;
enum Dev;

struct DervBonusMatrix{
	int derv[DERV_CAP];
};

struct Inventory{
	int hotbar_items[HOTBAR_SIZE];
	int inventory[INVENTORY_SIZE];
};

bool i_load_inventory(int global_index);
bool i_free_inventory(int global_index);
struct Inventory i_get_inv_proto(int gindx, bool autoload);
struct DervBonusMatrix i_get_bonus_matrx(int gindx, bool autoload);
