#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "t_config_tool.h"

// Rerwrite add a instnace manager with a generic object/instance struct which inherets properties from prototypes directly through the code.

typedef void (*Freer)(void *slot);
typedef void (*Init)(void *slot);
typedef void (*PostLoad)(void *slot);

struct ItemFunctions{
	ConfigLoader on_load;   // per-line config callback, called by t_config
	Init on_init;
	Freer on_free;
	PostLoad on_pload;
};
struct DepotManagerFile{
	int depot_count;
	int depot_item_cap[]; // flexible array. I'm not to happy about this but whatever
};

// The hierarchy is depot manager - depot- item

struct LoadData{
	char *base_path;
	char *format;
	void *loading_data;
};

struct DepotManager{
	void ***depots;
	int depot_cap;
	int *depot_item_caps;
	struct ItemFunctions *fncs;
};

struct DepotManagerFile *t_create_depotmanfile(int count);

struct DepotManager *t_create_depot_manager(struct DepotManagerFile *file);
void t_free_depot_manager(struct DepotManager *depot_manager);

void t_load_depot(struct DepotManager *depot_manager, int depot_index, struct ItemFunctions fncs);
void t_free_depot(struct DepotManager *depot_manager, int depot_index);

void *t_load_item(struct DepotManager *depot_manager, int depot_index, int item_index, size_t size);
void t_free_item(struct DepotManager *depot_manager, int depot_index, int item_index);

void *t_grab_item(struct DepotManager *depot_manager, int depot_index, int item_index, size_t size);

