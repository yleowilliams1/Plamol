#pragma once
#include <stdio.h>
#include "t_config_tool.h"

// We seperate between structs
// which need to track gindx-lindx
// and ones which don't with Si
// (non gindx-lindx) and Dou 
// (with gindx-lindx tracking)


enum DouEnum{
	DOU_INV,
	DOU_ITEM,
	DOU_ENTIT,
	DOU_LOOT,
	DOU_STAT,
	DOU_SPRITE,
	DOU_COUNT,
};

struct DouLoader{
	Loader on_load;
	Init   on_init;
	Freer  on_free;	
	PostLoad on_pload;
	size_t size;
	enum DouEnum type;
};
struct DouFunctions{	
	Loader on_load;
	Init   on_init;
	Freer  on_free;	
	PostLoad on_pload;
};
struct MemDou{
	void *iarr;
	struct local_indx *iman;
	int icount;
	size_t size;
};
struct DouManager{
	struct MemDou dou[DOU_COUNT];
	struct DouFunctions dou_fnc[DOU_COUNT];
};

char *e_dou_to_str(enum DouEnum type);

// Need grab 
// Need load 
// Need free 
// Need instantiate 
