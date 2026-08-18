#include <stdio.h>
#include <stdbool.h>

#include "e_engine_settings.h"

#include "dou_proto_inventory.h"
#include "dou_proto_items.h"
#include "dou_proto_entity.h"
#include "dou_proto_loot.h"
#include "dou_proto_stats.h"

#include "t_log_handler.h"
#include "e_dou_manager.h"
#include "t_gindex_tool.h"

char *e_dou_to_str(enum DouEnum type){
	if(type < 0 || type >= DOU_COUNT){
		LOG(LOG_NULL, "%d is not a valid enum", type);
		return "";
	}
	const char *lokup[DOU_COUNT] = {
		[DOU_INV] = "Inventory",
		[DOU_ITEM] = "Item",
		[DOU_ENTIT] = "Entity",
		[DOU_LOOT] = "Loot",
		[DOU_STAT] = "Stat",
		[DOU_SPRITE] = "Sprite",
	};

	return (char *)lokup[type];
}

bool e_load_dou(struct DouManager *dou, struct DouLoader lod){
	if(!dou){LOG(LOG_NULL, "Can't load dou passed NULL pointer"); return false;}	
	if(lod.type < 0 || lod.type >= DOU_COUNT){LOG(LOG_NULL, "%d is not a valid type", lod.type); return false;}	
	
	struct MemDou *mem = &dou->dou[lod.type];
	struct DouFunctions *fnc = &dou->dou_fnc[lod.type];

	if(mem->iarr || mem->iman){LOG(LOG_RELOAD, "Can't load %s since already loaded", e_dou_to_str(lod.type)); return false;}
	
	mem->icount = e_grab_doucount(lod.type);
	mem->size = lod.size;

	mem->iman = XCALLOC(1,  sizeof(struct local_indx) * mem->icount);
	mem->iarr = XCALLOC(1, mem->size * mem->icount);
	
	fnc->on_load = lod.on_load;
	fnc->on_init = lod.on_init;
	fnc->on_free = lod.on_free;
	fnc->on_pload = lod.on_pload;
	
	LOG(LOG_LOAD, "Loaded %s", e_dou_to_str(lod.type));

	return true;
}
bool e_unload_dou(struct DouManager *dou, enum DouEnum type){
	if(!dou){LOG(LOG_NULL, "Can't load dou passed NULL pointer"); return false;}	
	if(type < 0 || type >= DOU_COUNT){LOG(LOG_NULL, "%d is not a valid type", type); return false;}	
	
	struct MemDou *mem = &dou->dou[type];
	struct DouFunctions *fnc = &dou->dou_fnc[type];
	
	if(mem->iman){free(mem->iman); mem->iman = NULL;;}

	if(mem->iarr){
		for(int i = 0; i < mem->icount; i++){
			void *slot = (char *)mem->iarr + (size_t)i * mem->size;
        		fnc->on_free(slot);
		}
		free(mem->iarr);
		mem->iarr = NULL;
	}

	fnc->on_load = NULL;
	fnc->on_init = NULL;
	fnc->on_free = NULL;
	fnc->on_pload = NULL;
	
	LOG(LOG_FREE, "freed %s", e_dou_to_str(type));

	return true;	
}
struct DouManager *e_create_dou_manager(){
	struct DouManager *mem = XCALLOC(1, sizeof(struct DouManager));
	LOG(LOG_LOAD, "Loaded Memory Manager");
	return mem;
}
void e_free_dou_manager(struct DouManager *mem){
	if(!mem){LOG(LOG_RELOAD, "Can't free NULL memory manager");return;}
	
	for(int i = 0; i < DOU_COUNT; i++){
		e_unload_dou(mem, i);
	}

	free(mem);
	mem = NULL;
	LOG(LOG_FREE, "Freed memory manager");
}
bool e_load_dou_data(struct DouManager *mem ,enum DouEnum dou, int gindx){
	struct MemDou *data = &mem->dou[dou];
	if(!data->iarr){LOG(LOG_NULL, "iarr for %s is NULL", e_dou_to_str(dou)); return false;}
	if(!data->iman){LOG(LOG_NULL, "iman for %s is NULL", e_dou_to_str(dou)); return false;}

	struct DouLoadData load = {
		.gindx = gindx,
		.cap = data->icount,
		.iman = data->iman,
		.iarr = data->iarr,
		.element_size = data->size,
		.path = e_grab_doupath(dou),
		.format = e_grab_douformat(dou),
		.loader = mem->dou_fnc[dou].on_load,
		.init = mem->dou_fnc[dou].on_init,	
		.pload = mem->dou_fnc[dou].on_pload,
	};
	bool loaded = t_handle_dou_loading(load);	
	if(!loaded){LOG(LOG_RELOAD, "Failed to load gindx %d of %s", gindx, e_dou_to_str(dou)); return false;}

	LOG(LOG_LOAD, "Loaded gindx %d of dou %s with path %s and format %s", gindx, e_dou_to_str(dou), e_grab_doupath(dou), e_grab_douformat(dou));
	return true;
}
bool e_free_dou_data(struct DouManager *mem ,enum DouEnum dou, int gindx){
	struct MemDou *data = &mem->dou[dou];
	if(!data->iarr){LOG(LOG_NULL, "iarr for %s is NULL", e_dou_to_str(dou)); return false;}
	if(!data->iman){LOG(LOG_NULL, "iman for %s is NULL", e_dou_to_str(dou)); return false;}

	struct DouFreeData free = {
		.lindx = NULL,
		.gindx = &gindx,
		.cap = data->icount,
		.iman = data->iman,
		.iarr = data->iarr,
		.element_size = data->size,
		.freer= mem->dou_fnc[dou].on_free,	
	};
	bool freed = t_handle_dou_freeing(free);	
	if(!freed){LOG(LOG_RELOAD, "Failed to free gindx %d of %s", gindx, e_dou_to_str(dou)); return false;}

	LOG(LOG_FREE, "Freed gindx %d of dou %s ", gindx, e_dou_to_str(dou));
	return true;
}
