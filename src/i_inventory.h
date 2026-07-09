#pragma once
#include <stdbool.h>

#define HOTBAR_SIZE 8
#define INVENTORY_SIZE 32
enum Stats;
enum Dev;
struct Inventory{
	int hotbar_items[HOTBAR_SIZE];
	int inventory[INVENTORY_SIZE];
	int global_index;
	bool active; 
};

bool i_load_inventory(int global_index);
bool i_free_inventory(int global_index);
int i_grab_stat_bonus(enum Stats stat, int indx);
int i_grab_dev_bonus(enum Dev dev, int indx);
