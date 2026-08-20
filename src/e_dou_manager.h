#pragma once
#include <stdio.h>
#include <stdbool.h>

#include "c_flag_enums.h"
#include "t_pool.h"
#include "t_config_tool.h"

// We seperate between structs
// which need to track gindx-lindx
// and ones which don't with Si
// (non gindx-lindx) and Dou 
// (with gindx-lindx tracking)

struct InRef;

struct DouFunctions{	
	Loader on_load;
	Init   on_init;
	Freer  on_free;	
	PostLoad on_pload;
};
struct DouLoader{
	struct DouFunctions func;
	size_t size;
	enum DouFlag type;
};
struct DouManager{
	struct Pool pools[DOU_COUNT];
	struct DouFunctions dou_fnc[DOU_COUNT];
};

char *e_dou_to_str(enum DouFlag type);

struct DouManager *e_create_dou_manager();
void e_free_dou_manager(struct DouManager *dou);

void e_load_dou_pool(struct DouManager *dou, struct DouLoader ldr);
void e_free_dou_pool(struct DouManager *dou, enum DouFlag type);

struct InRef e_load_dou_item(struct DouManager *dou, enum DouFlag type, int gindx, void *out);
bool e_free_dou_item(struct DouManager *dou, enum DouFlag type, struct InRef item);
