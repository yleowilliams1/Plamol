#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "e_engine_settings.h"

#include "si_save_manager.h"

#include "t_depot_manager.h"
#include "t_log_handler.h"
#include "t_strings.h"
#include "t_config_tool.h"

#include "c_depot_list.h"


// write windows posix thing...

// We keep this constant since it's probably a bad idea of have the player be able to change where the game saves from..
#define SAVE_BASE_DIR "data/saves/items"
#define SLOT_BASE_DIR "data/saves/slots"
static bool valid_slot(int save);
static bool valid_map(int map_gindx);
static int find_highest_save_index(const char *dir_path);

	
struct SaveManager *si_create_save_manager(){

	struct SaveManager *sm = XCALLOC(1, sizeof(struct SaveManager));
	sm->slot_cursor = 0;
	
	// Gentle reminder that -1 items will just load the default map	
	for(int i = 0; i < SAVE_SLOT_COUNT; i++){
		sm->slot[i]->item_indexs = XCALLOC(1, sizeof(int) * e_grab_mapcount());
	}

	LOG(LOG_LOAD, "Created save manager with %d slots", SAVE_SLOT_COUNT);
	return sm;
}

void si_free_save_manager(struct SaveManager *sm){
	if(!sm){LOG(LOG_FREE, "Can't free NULL save manager"); return;}

	for(int i = 0; i < SAVE_SLOT_COUNT; i++){
		si_free_save_file(sm->slot[i]);
		sm->slot[i] = NULL;
	}

	free(sm);
	sm = NULL;
	LOG(LOG_FREE, "Freed save manager");
}

void si_update_save_cursor(struct SaveManager *sm, int new_cursor){
	if(!sm){LOG(LOG_NULL, "Save manager has not been created yet"); return;}
	if(!valid_slot(new_cursor)){LOG(LOG_OUTOFBOUNDS, "%d is not a valid save slot", new_cursor); return;}

	sm->slot_cursor = new_cursor;
	LOG(LOG_SET, "Save cursor moved to slot %d", new_cursor);
}
void si_free_save_file(struct SaveFile *sf){
	if(!sf){LOG(LOG_NULL, "Can't free sf since it's null");return;}
	free(sf->item_indexs);
	sf->item_indexs = NULL;
}

// Folder structure is as follows
// - Data
// 	- Saves
// 		-n_map
// 			-x.save
// The idea is to save some disk space and process time by keeping map saves which haven't changed.
// The function below saves a item, or the x.save, so it's path is BASE_SAVE_DIR/map_gindx_map/x.save
// with x being a new save in the list.
//
// slot = a list of paths to save items
// item = a single binary blob of a depot manager linked to a specific map at a specic time
void si_save_map_depot(struct SaveManager *sm, int map_gindx, struct DepotManager *dman){
	if(!dman){LOG(LOG_NULL, "Can't save, depot manager is NULL"); return;}
	if(!sm){LOG(LOG_NULL, "Save manager has not been created yet"); return;}
	if(!valid_map(map_gindx)){LOG(LOG_OUTOFBOUNDS, "Map has an invalid gindx (%d)", map_gindx); return;}
		
	size_t bufsize = 256;
	char buf[bufsize];
	t_snprintf(buf, bufsize, NULL, "%s/%d_%s", SAVE_BASE_DIR, map_gindx, "map");	

	int save_item_index = find_highest_save_index(buf);
	if(save_item_index <= -1){LOG(LOG_NULL, "Saveitem index was -1");return;}	
	char path[bufsize];
	t_snprintf(path, bufsize, NULL, "%s/%d.save", buf, save_item_index);	
	
	t_save_depot(dman, DPO_ENTITY_INST, path);
	t_save_depot(dman, DPO_INTER_INST, path);
}
void si_load_map_depot(struct SaveManager *sm, int map_gindx, int item_gindx, struct DepotManager *dman){
	if(!dman){LOG(LOG_NULL, "Can't save, depot manager is NULL"); return;}
	if(!sm){LOG(LOG_NULL, "Save manager has not been created yet"); return;}
	if(!valid_map(map_gindx)){LOG(LOG_OUTOFBOUNDS, "Map has an invalid gindx (%d)", map_gindx); return;}
	if(item_gindx == -1){return;}	
	size_t bufsize = 256;
	char path[bufsize];
	t_snprintf(path, bufsize, NULL, "%s/%d_%s/%d.save", SAVE_BASE_DIR, map_gindx, "map", item_gindx);	
	
	t_read_depot(dman, DPO_ENTITY_INST, path);
	t_read_depot(dman, DPO_INTER_INST, path);
}
void si_set_to_slot(struct SaveManager *sm, int map_gindx, int item_gindx, int save_slot){
	if(!valid_slot(save_slot)){LOG(LOG_NULL, "Invalid save slot %d", save_slot);return;}
	if(!sm){LOG(LOG_NULL, "Save manager has not been created yet"); return;}
	if(!valid_map(map_gindx)){LOG(LOG_OUTOFBOUNDS, "Map has an invalid gindx (%d)", map_gindx); return;}
	
	sm->slot[save_slot]->item_indexs[map_gindx] = item_gindx;	
}
void si_clear_to_slot(struct SaveManager *sm, int map_gindx, int save_slot){
	if(!valid_slot(save_slot)){LOG(LOG_NULL, "Invalid save slot %d", save_slot);return;}
	if(!sm){LOG(LOG_NULL, "Save manager has not been created yet"); return;}
	if(!valid_map(map_gindx)){LOG(LOG_OUTOFBOUNDS, "Map has an invalid gindx (%d)", map_gindx); return;}
	
	// It just means that you load the default if it gets to it.
	sm->slot[save_slot]->item_indexs[map_gindx] = -1;	
}
static bool valid_slot(int save){
	return save >= 0 && save < SAVE_SLOT_COUNT;
}
static bool valid_map(int map_gindx){
	return map_gindx < 0  && map_gindx > e_grab_mapcount();
}
static int find_highest_save_index(const char *dir_path){
	DIR *dir = opendir(dir_path);
	if(!dir){return 0;} 

	int highest = -1;
	struct dirent *entry;
	while((entry = readdir(dir)) != NULL){
		int n;
		char extra[8];
		if(sscanf(entry->d_name, "%d.save%7s", &n, extra) == 1){
			if(n > highest){highest = n;}
		}
	}
	closedir(dir);
	return highest;
}
