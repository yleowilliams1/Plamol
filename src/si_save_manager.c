#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

#include "e_engine_settings.h"

#include "si_save_manager.h"

#include "t_instance_manager.h"
#include "t_log_handler.h"
#include "t_strings.h"
#include "t_config_tool.h"

#include "c_instance_list.h"


// write windows posix thing...

// We keep this constant since it's probably a bad idea of have the player be able to change where the game saves from..
#define SAVE_BASE_DIR "data/saves/items"
#define SLOT_BASE_DIR "data/saves/slots"
static bool valid_slot(int save);
static bool valid_map(int map_gindx);
static int find_highest_save_index(const char *dir_path);
static void ensure_dir_exists(const char *dir_path);


struct SaveManager *si_create_save_manager(){

	struct SaveManager *sm = XCALLOC(1, sizeof(struct SaveManager));
	sm->slot_cursor = 0;
	// Gentle reminder that -1 items will just load the default map
	for(int i = 0; i < SAVE_SLOT_COUNT; i++){
		sm->slot[i] = XCALLOC(1, sizeof(struct SaveFile));
		sm->slot[i]->item_indexs = XCALLOC(1, sizeof(int) * e_grab_mapcount());
		for(int m = 0; m < e_grab_mapcount(); m++){
			sm->slot[i]->item_indexs[m] = -1;
		}
	}

	LOG(LOG_LOAD, "Created save manager with %d slots", SAVE_SLOT_COUNT);
	return sm;
}

void si_free_save_manager(struct SaveManager *sm){
	if(!sm){LOG(LOG_FREE, "Can't free NULL save manager"); return;}

	for(int i = 0; i < SAVE_SLOT_COUNT; i++){
		si_free_save_file(sm->slot[i]);
		free(sm->slot[i]);
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

int si_get_active_item_index(struct SaveManager *sm, int map_gindx){
	if(!sm){LOG(LOG_NULL, "Save manager has not been created yet"); return -1;}
	if(!valid_map(map_gindx)){LOG(LOG_OUTOFBOUNDS, "Map has an invalid gindx (%d)", map_gindx); return -1;}
	if(!valid_slot(sm->slot_cursor)){LOG(LOG_OUTOFBOUNDS, "Save cursor %d is not a valid slot", sm->slot_cursor); return -1;}

	return sm->slot[sm->slot_cursor]->item_indexs[map_gindx];
}

// Folder structure is as follows
// - Data
// 	- Saves
// 		-n_map
// 			-x_INST_TYPE.save
// The idea is to save some disk space and process time by keeping map saves which haven't changed.
// Entity and interactable instances get their own files (per instance type) so that saving
// one doesn't clobber the other - they used to share a path and overwrite each other.
char *si_resolve_instance_path(struct SaveManager *sm, int map_gindx, enum InstanceType type){
	int item_gindx = si_get_active_item_index(sm, map_gindx);
	if(item_gindx < 0){return NULL;}

	size_t bufsize = 256;
	char *path = XCALLOC(1, bufsize);
	t_snprintf(path, bufsize, NULL, "%s/%d_%s/%d_%s.save", SAVE_BASE_DIR, map_gindx, "map", item_gindx, (char *)inststr(type));
	return path;
}

void si_load_stage_instances(struct SaveManager *sm, int map_gindx, struct DepotManager *dman,
	struct InstanceManager *entities, struct InstanceSlot *entity_slots, int entity_slot_count, int entity_depot_index,
	struct InstanceManager *interactables, struct InstanceSlot *interactable_slots, int interactable_slot_count, int interactable_depot_index){

	if(!sm){LOG(LOG_NULL, "Save manager has not been created yet"); return;}
	if(!dman){LOG(LOG_NULL, "Can't populate stage instances, depot manager is NULL"); return;}

	char *entity_path = si_resolve_instance_path(sm, map_gindx, INST_ENTITY);
	t_populate_instance_manager(entities, dman, entity_depot_index, entity_slots, entity_slot_count, entity_path);
	free(entity_path);

	char *interactable_path = si_resolve_instance_path(sm, map_gindx, INST_INTERACTABLE);
	t_populate_instance_manager(interactables, dman, interactable_depot_index, interactable_slots, interactable_slot_count, interactable_path);
	free(interactable_path);
}

void si_save_map_instances(struct SaveManager *sm, int map_gindx, struct InstanceManager *entities, struct InstanceManager *interactables){
	if(!sm){LOG(LOG_NULL, "Save manager has not been created yet"); return;}
	if(!valid_map(map_gindx)){LOG(LOG_OUTOFBOUNDS, "Map has an invalid gindx (%d)", map_gindx); return;}

	size_t bufsize = 256;
	char dir[bufsize];
	t_snprintf(dir, bufsize, NULL, "%s/%d_%s", SAVE_BASE_DIR, map_gindx, "map");
	ensure_dir_exists(dir);

	int save_item_index = find_highest_save_index(dir) + 1;

	char entity_path[bufsize];
	t_snprintf(entity_path, bufsize, NULL, "%s/%d_%s.save", dir, save_item_index, (char *)inststr(INST_ENTITY));
	t_save_instance_manager(entities, entity_path);

	char interactable_path[bufsize];
	t_snprintf(interactable_path, bufsize, NULL, "%s/%d_%s.save", dir, save_item_index, (char *)inststr(INST_INTERACTABLE));
	t_save_instance_manager(interactables, interactable_path);

	LOG(LOG_LOAD, "Saved map %d instances as item %d", map_gindx, save_item_index);
}

void si_load_map_instances(struct SaveManager *sm, int map_gindx, int item_gindx, struct InstanceManager *entities, struct InstanceManager *interactables){
	if(!sm){LOG(LOG_NULL, "Save manager has not been created yet"); return;}
	if(!valid_map(map_gindx)){LOG(LOG_OUTOFBOUNDS, "Map has an invalid gindx (%d)", map_gindx); return;}
	if(item_gindx < 0){return;}

	size_t bufsize = 256;
	char entity_path[bufsize];
	t_snprintf(entity_path, bufsize, NULL, "%s/%d_%s/%d_%s.save", SAVE_BASE_DIR, map_gindx, "map", item_gindx, (char *)inststr(INST_ENTITY));
	t_load_instance_manager(entities, entity_path);

	char interactable_path[bufsize];
	t_snprintf(interactable_path, bufsize, NULL, "%s/%d_%s/%d_%s.save", SAVE_BASE_DIR, map_gindx, "map", item_gindx, (char *)inststr(INST_INTERACTABLE));
	t_load_instance_manager(interactables, interactable_path);
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
	return map_gindx >= 0 && map_gindx < e_grab_mapcount();
}
// Returns -1 if the dir doesn't exist yet or has no save files (i.e. "no saves").
static int find_highest_save_index(const char *dir_path){
	DIR *dir = opendir(dir_path);
	if(!dir){return -1;} 

	int highest = -1;
	struct dirent *entry;
	while((entry = readdir(dir)) != NULL){
		int n;
		char extra[64];
		if(sscanf(entry->d_name, "%d_%63s", &n, extra) == 2){
			if(n > highest){highest = n;}
		}
	}
	closedir(dir);
	return highest;
}
// Minimal mkdir -p. Best-effort: logs but doesn't hard-fail, since the actual
// fopen() call downstream will fail loudly enough if this didn't work.
static void ensure_dir_exists(const char *dir_path){
	char buf[256];
	size_t len = strlen(dir_path);
	if(len == 0 || len >= sizeof(buf)){return;}
	strcpy(buf, dir_path);

	for(char *p = buf + 1; *p; p++){
		if(*p != '/'){continue;}
		*p = '\0';
		if(mkdir(buf, 0755) != 0 && errno != EEXIST){
			LOG(LOG_NULL, "Failed to create directory %s", buf);
		}
		*p = '/';
	}
	if(mkdir(buf, 0755) != 0 && errno != EEXIST){
		LOG(LOG_NULL, "Failed to create directory %s", buf);
	}
}
