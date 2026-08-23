#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "t_config_tool.h"

typedef void (*Loader)(void *loaddata, void *ptr);
typedef void (*Freer)(void *slot);
typedef void (*Init)(void *slot);
typedef void (*PostLoad)(void *slot);

struct ItemFunctions{
	ConfigLoader on_load;   // per-line config callback, called by t_config
    	Loader on_bulk;
	Init on_init;
	Freer on_free;
	PostLoad on_pload;
};
struct RegisterFile{
	int register_cap;
	int register_item_cap[]; // flexible array. I'm not to happy about this but whatever
};

// The hierarchy is manager - register - item

struct LoadData{
	char *base_path;
	char *format;
	void *loading_data;
};

struct RegisterManager{
	void ***registers;
	int register_cap;
	int *register_item_caps;
	struct ItemFunctions *fncs;
};

struct RegisterManager *e_create_rmanager();
void e_free_rmanager(struct RegisterManager *rman);

void e_load_register(struct RegisterManager *rman, int reg, struct ItemFunctions fncs);
void e_free_register(struct RegisterManager *rman, int reg);

void *e_load_item(struct RegisterManager *rman, int reg, struct LoadData, int gindx, size_t size);
void e_free_item(struct RegisterManager *rman, int reg, int gindx);
void *e_grab_item(struct RegisterManager *rman, int reg, int gindx);
