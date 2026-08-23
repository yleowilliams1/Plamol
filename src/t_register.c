#include <stdio.h>
#include <stdbool.h>

#include "t_register.h"
#include "t_log_handler.h"
#include "t_config_tool.h"

struct RegisterFile *e_regfile_create(int cap){
	struct RegisterFile *rf = XCALLOC(1, sizeof *rf + (size_t)cap * sizeof(int));
	rf->register_cap = cap;
	return rf;
}

struct RegisterManager *e_create_rmanager(struct RegisterFile *file){
	if(!file){return NULL;}
	struct RegisterManager *rman = XCALLOC(1, sizeof(struct RegisterManager));
	
	rman->register_cap = file->register_cap;
	if(rman->register_cap <= 0){LOG(LOG_NULL, "%d Is not a valid register_cap", rman->register_cap); rman->register_cap = 1;}
	
	rman->registers = XCALLOC(1, sizeof(void **) * rman->register_cap);
	rman->register_item_caps = XCALLOC(1, sizeof(int) * rman->register_cap);
	rman->fncs = XCALLOC(1, sizeof(struct ItemFunctions) * rman->register_cap);

	for(int i = 0; i < rman->register_cap; i++){
		rman->register_item_caps[i] = file->register_item_cap[i];
		if(rman->register_item_caps[i] <= 0){
			LOG(LOG_NULL, "%d is an invalid itemcap for type index %d", rman->register_item_caps[i], i); 
			rman->register_item_caps[i] = 1;
		}
		rman->registers[i] = XCALLOC(1, sizeof(void *) * rman->register_item_caps[i]);	
	}

	free(file);
	file = NULL;
	return rman;
}
void e_free_rmanager(struct RegisterManager *rman){
	if(!rman){LOG(LOG_NULL, "Can't free null register manager");return;}
	for(int i = 0; i < rman->register_cap; i++){
		if(rman->registers[i] == NULL){continue;}
		e_free_register(rman, i);
	}

	free(rman->register_item_caps);
	free(rman->fncs);
	free(rman->registers);
	free(rman);
	rman= NULL;
}
void e_load_register(struct RegisterManager *rman, int reg, struct ItemFunctions fncs){
	if(!rman){LOG(LOG_NULL, "Can't load register %d with NULL register manager", reg);return;}
	if(reg < 0 || reg >= rman->register_cap){LOG(LOG_NULL, "%d is notr a valid register index", reg); return;}
	if(rman->registers[reg]){LOG(LOG_RELOAD, "Can't load %d register since it's not NULL", reg); return;}
	if(!rman->fncs){LOG(LOG_RELOAD, "Can't load %d register since fncs is NULL", reg); return;}
	if(!rman->register_item_caps){LOG(LOG_RELOAD, "Can't load %d register since item_caps array is NULL", reg); return;}

	rman->registers[reg] = XCALLOC(1, sizeof(void *) * rman->register_item_caps[reg]);
	rman->fncs[reg] = fncs;	
}
void e_free_register(struct RegisterManager *rman, int reg){		
	if(!rman){LOG(LOG_NULL, "Can't free register %d with NULL register manager", reg);return;}
	if(reg < 0 || reg >= rman->register_cap){LOG(LOG_NULL, "%d is notr a valid register index", reg); return;}
	if(!rman->registers[reg]){LOG(LOG_RELOAD, "Can't free %d register since it's NULL", reg); return;}
	if(!rman->fncs){LOG(LOG_RELOAD, "Can't free %d register since fncs is NULL", reg); return;}
	if(!rman->register_item_caps){LOG(LOG_RELOAD, "Can't free %d register since item_caps array is NULL", reg); return;}


	for(int i = 0; i < rman->register_item_caps[reg]; i++){
		if(!rman->registers[reg][i]){continue;}
		e_free_item(rman, reg, i);
	}	
	
	free(rman->registers[reg]);
	rman->registers[reg] = NULL;
	rman->fncs[reg] = (struct ItemFunctions){0};	
}
void *e_load_item(struct RegisterManager *rman, int reg, struct LoadData ld, int gindx, size_t size){
	if(!rman){LOG(LOG_NULL, "Can't load item %d of reg %d with NULL register manager", gindx, reg);return NULL;}
	if(reg < 0 || reg >= rman->register_cap){LOG(LOG_NULL, "%d is not a valid register index", reg); return NULL;}
	if(!rman->registers[reg]){LOG(LOG_RELOAD, "Can't load item %d of register %d since the register is NULL", gindx, reg); return NULL;}
	if(!rman->fncs){LOG(LOG_RELOAD, "Can't load item %d of register %d since fncs is NULL", gindx, reg); return NULL;}
	if(!rman->register_item_caps){LOG(LOG_RELOAD, "Can't load item %d of %d register since item_caps array is NULL", gindx, reg); return NULL;}
	if(gindx < 0 || gindx >= rman->register_item_caps[reg]){LOG(LOG_NULL, "%d is not a valid item index for register %d", gindx, reg); return NULL;}
	if(rman->registers[reg][gindx]){LOG(LOG_RELOAD, "Item %d of register %d is already loaded", gindx, reg); return NULL;}

	struct ItemFunctions *fncs = &rman->fncs[reg];

	void *item = XCALLOC(1, size);
	if(fncs->on_init){fncs->on_init(item);}

	if(fncs->on_bulk && ld.loading_data){fncs->on_bulk(ld.loading_data, item);}
	
	if(ld.base_path && ld.format){
		char *full_path = t_format_path(ld.base_path, ld.format, gindx);
		if(!full_path){
			if(fncs->on_free){fncs->on_free(item);}
			free(item);
			return NULL;
		}

		bool configured = t_config(item, full_path, fncs->on_load);
		free(full_path);
		if(!configured){
			LOG(LOG_NULL, "Failed to configure gindx %d of register %d with base %s and format %s", gindx, reg, ld.base_path, ld.format);
			if(fncs->on_free){fncs->on_free(item);}
			free(item);
			return NULL;
		}
	}
	
	if(fncs->on_pload){fncs->on_pload(item);}

	rman->registers[reg][gindx] = item;   // <-- the missing write
	return item;
}

void e_free_item(struct RegisterManager *rman, int reg, int gindx){
	if(!rman){LOG(LOG_NULL, "Can't free item %d of reg %d with NULL register manager", gindx, reg);return;}
	if(reg < 0 || reg >= rman->register_cap){LOG(LOG_NULL, "%d is not a valid register index", reg); return;}
	if(!rman->registers[reg]){LOG(LOG_RELOAD, "Can't free item %d of register %d since the register is NULL", gindx, reg); return;}
	if(!rman->fncs){LOG(LOG_RELOAD, "Can't free item %d of register %d since fncs is NULL", gindx, reg); return;}
	if(!rman->register_item_caps){LOG(LOG_RELOAD, "Can't free item %d of %d register since item_caps array is NULL", gindx, reg); return;}
	if(gindx < 0 || gindx >= rman->register_item_caps[reg]){LOG(LOG_NULL, "%d is not a valid item index for register %d", gindx, reg); return;}

	void *item = rman->registers[reg][gindx];   // the actual item, not &slot
	if(!item){LOG(LOG_RELOAD, "Tried to free item %d of register %d but it's already NULL", gindx, reg); return;}

	struct ItemFunctions *fncs = &rman->fncs[reg];
	if(fncs->on_free){fncs->on_free(item);}
	free(item);
	rman->registers[reg][gindx] = NULL;
}
