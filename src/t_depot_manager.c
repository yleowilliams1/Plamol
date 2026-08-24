#include <stdio.h>
#include <stdbool.h>

#include "t_depot_manager.h"
#include "t_log_handler.h"
#include "t_config_tool.h"

struct DepotManagerFile *t_create_depotmanfile(int count){
	struct DepotManagerFile *df = XCALLOC(1, sizeof *df + (size_t)count * sizeof(int));
	df->depot_count = count;
	return df;
}

struct DepotManager *t_create_depot_manager(struct DepotManagerFile *file){
	if(!file){return NULL;}
	struct DepotManager *depot_manager = XCALLOC(1, sizeof(struct DepotManager));

	depot_manager->depot_cap = file->depot_count;
	if(depot_manager->depot_cap <= 0){LOG(LOG_NULL, "%d is not a valid depot_cap", depot_manager->depot_cap); depot_manager->depot_cap = 1;}

	depot_manager->depots = XCALLOC(1, sizeof(void **) * depot_manager->depot_cap);
	depot_manager->depot_item_caps = XCALLOC(1, sizeof(int) * depot_manager->depot_cap);
	depot_manager->fncs = XCALLOC(1, sizeof(struct ItemFunctions) * depot_manager->depot_cap);

	for(int i = 0; i < depot_manager->depot_cap; i++){
		depot_manager->depot_item_caps[i] = file->depot_item_cap[i];
		if(depot_manager->depot_item_caps[i] <= 0){
			LOG(LOG_NULL, "%d is an invalid itemcap for type index %d", depot_manager->depot_item_caps[i], i);
			depot_manager->depot_item_caps[i] = 1;
		}
		depot_manager->depots[i] = XCALLOC(1, sizeof(void *) * depot_manager->depot_item_caps[i]);
	}

	free(file);
	file = NULL;
	return depot_manager;
}

void t_free_depot_manager(struct DepotManager *depot_manager){
	if(!depot_manager){LOG(LOG_NULL, "Can't free null depot manager");return;}
	for(int i = 0; i < depot_manager->depot_cap; i++){
		if(depot_manager->depots[i] == NULL){continue;}
		t_free_depot(depot_manager, i);
	}

	free(depot_manager->depot_item_caps);
	free(depot_manager->fncs);
	free(depot_manager->depots);
	free(depot_manager);
	depot_manager = NULL;
}

void t_load_depot(struct DepotManager *depot_manager, int depot_index, struct ItemFunctions fncs){
	if(!depot_manager){LOG(LOG_NULL, "Can't load depot %d with NULL depot manager", depot_index);return;}
	if(depot_index < 0 || depot_index >= depot_manager->depot_cap){LOG(LOG_NULL, "%d is not a valid depot index", depot_index); return;}
	if(depot_manager->depots[depot_index]){LOG(LOG_RELOAD, "Can't load %d depot since it's not NULL", depot_index); return;}
	if(!depot_manager->fncs){LOG(LOG_RELOAD, "Can't load %d depot since fncs is NULL", depot_index); return;}
	if(!depot_manager->depot_item_caps){LOG(LOG_RELOAD, "Can't load %d depot since item_caps array is NULL", depot_index); return;}

	depot_manager->depots[depot_index] = XCALLOC(1, sizeof(void *) * depot_manager->depot_item_caps[depot_index]);
	depot_manager->fncs[depot_index] = fncs;
}

void t_free_depot(struct DepotManager *depot_manager, int depot_index){
	if(!depot_manager){LOG(LOG_NULL, "Can't free depot %d with NULL depot manager", depot_index);return;}
	if(depot_index < 0 || depot_index >= depot_manager->depot_cap){LOG(LOG_NULL, "%d is not a valid depot index", depot_index); return;}
	if(!depot_manager->depots[depot_index]){LOG(LOG_RELOAD, "Can't free %d depot since it's NULL", depot_index); return;}
	if(!depot_manager->fncs){LOG(LOG_RELOAD, "Can't free %d depot since fncs is NULL", depot_index); return;}
	if(!depot_manager->depot_item_caps){LOG(LOG_RELOAD, "Can't free %d depot since item_caps array is NULL", depot_index); return;}

	for(int i = 0; i < depot_manager->depot_item_caps[depot_index]; i++){
		if(!depot_manager->depots[depot_index][i]){continue;}
		t_free_item(depot_manager, depot_index, i);
	}

	free(depot_manager->depots[depot_index]);
	depot_manager->depots[depot_index] = NULL;
	depot_manager->fncs[depot_index] = (struct ItemFunctions){0};
}

void *t_load_item(struct DepotManager *depot_manager, int depot_index, struct LoadData ld, int item_index, size_t size){
	if(!depot_manager){LOG(LOG_NULL, "Can't load item %d of depot %d with NULL depot manager", item_index, depot_index);return NULL;}
	if(depot_index < 0 || depot_index >= depot_manager->depot_cap){LOG(LOG_NULL, "%d is not a valid depot index", depot_index); return NULL;}
	if(!depot_manager->depots[depot_index]){LOG(LOG_RELOAD, "Can't load item %d of depot %d since the depot is NULL", item_index, depot_index); return NULL;}
	if(!depot_manager->fncs){LOG(LOG_RELOAD, "Can't load item %d of depot %d since fncs is NULL", item_index, depot_index); return NULL;}
	if(!depot_manager->depot_item_caps){LOG(LOG_RELOAD, "Can't load item %d of %d depot since item_caps array is NULL", item_index, depot_index); return NULL;}
	if(item_index < 0 || item_index >= depot_manager->depot_item_caps[depot_index]){LOG(LOG_NULL, "%d is not a valid item index for depot %d", item_index, depot_index); return NULL;}
	if(depot_manager->depots[depot_index][item_index]){LOG(LOG_RELOAD, "Item %d of depot %d is already loaded", item_index, depot_index); return NULL;}

	struct ItemFunctions *fncs = &depot_manager->fncs[depot_index];

	void *item = XCALLOC(1, size);
	if(fncs->on_init){fncs->on_init(item);}

	if(fncs->on_bulk && ld.loading_data){fncs->on_bulk(ld.loading_data, item);}

	if(ld.base_path && ld.format){
		char *full_path = t_format_path(ld.base_path, ld.format, item_index);
		if(!full_path){
			if(fncs->on_free){fncs->on_free(item);}
			free(item);
			return NULL;
		}

		bool configured = t_config(item, full_path, fncs->on_load);
		free(full_path);
		if(!configured){
			LOG(LOG_NULL, "Failed to configure item_index %d of depot %d with base %s and format %s", item_index, depot_index, ld.base_path, ld.format);
			if(fncs->on_free){fncs->on_free(item);}
			free(item);
			return NULL;
		}
	}

	if(fncs->on_pload){fncs->on_pload(item);}

	depot_manager->depots[depot_index][item_index] = item;   // <-- the missing write
	return item;
}

void t_free_item(struct DepotManager *depot_manager, int depot_index, int item_index){
	if(!depot_manager){LOG(LOG_NULL, "Can't free item %d of depot %d with NULL depot manager", item_index, depot_index);return;}
	if(depot_index < 0 || depot_index >= depot_manager->depot_cap){LOG(LOG_NULL, "%d is not a valid depot index", depot_index); return;}
	if(!depot_manager->depots[depot_index]){LOG(LOG_RELOAD, "Can't free item %d of depot %d since the depot is NULL", item_index, depot_index); return;}
	if(!depot_manager->fncs){LOG(LOG_RELOAD, "Can't free item %d of depot %d since fncs is NULL", item_index, depot_index); return;}
	if(!depot_manager->depot_item_caps){LOG(LOG_RELOAD, "Can't free item %d of %d depot since item_caps array is NULL", item_index, depot_index); return;}
	if(item_index < 0 || item_index >= depot_manager->depot_item_caps[depot_index]){LOG(LOG_NULL, "%d is not a valid item index for depot %d", item_index, depot_index); return;}

	void *item = depot_manager->depots[depot_index][item_index];   // the actual item, not &slot
	if(!item){LOG(LOG_RELOAD, "Tried to free item %d of depot %d but it's already NULL", item_index, depot_index); return;}

	struct ItemFunctions *fncs = &depot_manager->fncs[depot_index];
	if(fncs->on_free){fncs->on_free(item);}
	free(item);
	depot_manager->depots[depot_index][item_index] = NULL;
}

void *t_grab_item(struct DepotManager *depot_manager, int depot_index, int item_index){
	if(!depot_manager){LOG(LOG_NULL, "Can't grab item %d of depot %d with NULL depot manager", item_index, depot_index);return NULL;}
	if(depot_index < 0 || depot_index >= depot_manager->depot_cap){LOG(LOG_NULL, "%d is not a valid depot index", depot_index); return NULL;}
	if(!depot_manager->depot_item_caps){LOG(LOG_RELOAD, "Can't grab item %d of %d depot since item_caps array is NULL", item_index, depot_index); return NULL;}
	if(!depot_manager->depots[depot_index]){LOG(LOG_RELOAD, "Can't grab %d depot since it's NULL", depot_index); return NULL;}
	if(item_index < 0 || item_index >= depot_manager->depot_item_caps[depot_index]){LOG(LOG_NULL, "%d is not a valid item index for depot %d", item_index, depot_index); return NULL;}
	if(!depot_manager->depots[depot_index][item_index]){LOG(LOG_RELOAD, "Item %d of depot %d is NULL", item_index, depot_index); return NULL;}

	return depot_manager->depots[depot_index][item_index];
}

void t_on_interact(struct DepotManager *depot_manager, void *interactdata, int depot_index, int item_index){
	if(!depot_manager){LOG(LOG_NULL, "Can't interact with item %d of depot %d with NULL depot manager", item_index, depot_index);return;}
	if(depot_index < 0 || depot_index >= depot_manager->depot_cap){LOG(LOG_NULL, "%d is not a valid depot index", depot_index); return;}
	if(!depot_manager->depots[depot_index]){LOG(LOG_RELOAD, "Can't interact with item %d of depot %d since the depot is NULL", item_index, depot_index); return;}
	if(!depot_manager->fncs){LOG(LOG_RELOAD, "Can't interact with item %d of depot %d since fncs is NULL", item_index, depot_index); return;}
	if(!depot_manager->depot_item_caps){LOG(LOG_RELOAD, "Can't interact with item %d of %d depot since item_caps array is NULL", item_index, depot_index); return;}
	if(item_index < 0 || item_index >= depot_manager->depot_item_caps[depot_index]){LOG(LOG_NULL, "%d is not a valid item index for depot %d", item_index, depot_index); return;}

	void *item = depot_manager->depots[depot_index][item_index];
	if(!item){LOG(LOG_RELOAD, "Tried to interact with item %d of depot %d but it's NULL", item_index, depot_index); return;}

	struct ItemFunctions *fncs = &depot_manager->fncs[depot_index];
	if(!fncs->on_interact){LOG(LOG_NULL, "Can't interact with item %d of depot %d since the function is NULL", item_index, depot_index); return;}
	fncs->on_interact(interactdata, item);
}
