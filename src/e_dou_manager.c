#include <stdio.h>
#include <stdbool.h>

#include "e_engine_settings.h"
#include "e_dou_manager.h"

#include "t_log_handler.h"

struct DouManager *e_create_dou_manager(){
	struct DouManager *dou = XCALLOC(1, sizeof(struct DouManager));
	return dou;
}
void e_free_dou_manager(struct DouManager *dou){
	for(int i = 0; i < DOU_COUNT; i++){
		// This already checks double free so it's fine
		t_pool_free(&dou->pools[i]);
	}	

	free(dou);
	dou = NULL;	
}

void e_load_dou_pool(struct DouManager *dou, struct DouLoader ldr){
	if(!dou){LOG(LOG_NULL, "Can't load dou type passed NULL pointer"); return ;}	
	if(ldr.type < 0 || ldr.type >= DOU_COUNT){LOG(LOG_NULL, "%d is not a valid type", ldr.type); return ;}		
	if(t_pool_loaded(&dou->pools[ldr.type])){
		LOG(LOG_RELOAD, "Reloading %s", e_dou_to_str(ldr.type));
		t_pool_free(&dou->pools[ldr.type]);
	}

	t_pool_create(&dou->pools[ldr.type], e_grab_doucap(ldr.type), ldr.size);	
	dou->dou_fnc[ldr.type] = ldr.func;

	LOG(LOG_LOAD, "Loaded dou %s with cap %d", e_dou_to_str(ldr.type), e_grab_doucap(ldr.type));
}

void e_free_dou_pool(struct DouManager *dou, enum DouFlag type){
	if(!dou){LOG(LOG_NULL, "Can't free dou type passed NULL pointer"); return ;}	
	if(type < 0 || type >= DOU_COUNT){LOG(LOG_NULL, "%d is not a valid type", type); return ;}		
	if(!t_pool_loaded(&dou->pools[type])){
		LOG(LOG_RELOAD, "Not loaded so can't free %s", e_dou_to_str(type));
		return;
	}

	t_pool_free(&dou->pools[type]);	
	
	dou->dou_fnc[type].on_load = NULL;
	dou->dou_fnc[type].on_init = NULL;
	dou->dou_fnc[type].on_free = NULL;
	dou->dou_fnc[type].on_pload = NULL;

	LOG(LOG_LOAD, "Free dou %s with cap %d", e_dou_to_str(type), e_grab_doucap(type));
}
struct InRef e_load_dou_item(struct DouManager *dou, enum DouFlag type, int gindx, void *out){
	if(!dou){LOG(LOG_NULL, "Can't load %s %d since Dou is null", e_dou_to_str(type), gindx); return (struct InRef){0};}
	if(!t_pool_loaded(&dou->pools[type])){LOG(LOG_NULL, "Dou %s is not initalized so can't load any data", e_dou_to_str(type));return (struct InRef){0};}
		
	out = NULL;

	struct InRef item;
	void *ptr = t_pool_alloc(&dou->pools[type], &item);
	if(!ptr){LOG(LOG_NULL, "Failed to allocate %s %d", e_dou_to_str(type), gindx); return (struct InRef){0};}
	
	// Initalize
	if(dou->dou_fnc[type].on_init){dou->dou_fnc[type].on_init(ptr);}	
	
	// Format the path	
	char *base_path = e_grab_doupath(type);
	char *format = e_grab_douformat(type);
	char *path = t_format_path(base_path, format, gindx);
	// Let it run. It either figures itself out later or crashes before it messes stuff up
	if(!path){LOG(LOG_NULL, "Failed to format path:%s format:%s gindx:%d", base_path, format, gindx);}
	bool configured = t_config(ptr, path, dou->dou_fnc[type].on_load);
	if(!configured){LOG(LOG_NULL, "Failed to configure %s of path %s", e_dou_to_str(type), path); free(path); return (struct InRef){0};}
	
	// Now pload
	if(dou->dou_fnc[type].on_pload){dou->dou_fnc[type].on_pload(ptr);}

	free(path);
	if(out){out = ptr;}	

	LOG(LOG_LOAD, "Loaded gindx %d of dou %s with path %s and format %s", gindx, e_dou_to_str(type), e_grab_doupath(type), e_grab_douformat(type));
	return item;
}
bool e_free_dou_item(struct DouManager *dou, enum DouFlag type, struct InRef item){
	if(!dou){LOG(LOG_NULL, "Can't load %s item since Dou is null", e_dou_to_str(type)); return false;}
	if(!t_pool_loaded(&dou->pools[type])){LOG(LOG_NULL, "Dou %s is not initalized so can't load any data", e_dou_to_str(type));return false;}	

	void *ptr = t_pool_get(&dou->pools[type], item);
	if(!ptr){LOG(LOG_NULL, "Pool get for slot %d gen %d returned NULL", item.slot, item.gen); return false;}
	if(dou->dou_fnc[type].on_free){dou->dou_fnc[type].on_free(ptr);}	
	bool released = t_pool_release(&dou->pools[type], item);
	if(!released){LOG(LOG_NULL, "Failed to free slot %d gen %d", item.slot, item.gen); return false;}
	return true;
}
