#pragma once
#include <stdbool.h>
#include "c_magic_number.h"

struct Inventory{
	int hotbar_items[HOTBAR_SIZE];
	int inventory[INVENTORY_SIZE];
};

struct DouLoader;

struct DouLoader dou_inventory();
