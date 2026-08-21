#pragma once
#include <stdio.h>
#include <stdbool.h>

#include "c_flag_enums.h"
#include "t_config_tool.h"

struct ItemFunctions{	
	Loader on_load;
	Init   on_init;
	Freer  on_free;	
	PostLoad on_pload;
};
struct PrototypeManager{
	void ***reg;
	struct ItemFunctions fncs[PROT_COUNT];
};

struct PrototypeManager *e_create_pmanager();
void e_free_pmanager(struct PrototypeManager *proto);

void e_load_register(struct PrototypeManager *proto, enum PrototypeFlag type, struct ItemFunctions fncs);
void e_free_register(struct PrototypeManager *proto, enum PrototypeFlag type);

void *e_load_item(struct PrototypeManager *proto, enum PrototypeFlag type, int gindx, size_t size);
void e_free_item(struct PrototypeManager *proto, enum PrototypeFlag type, int gindx);
