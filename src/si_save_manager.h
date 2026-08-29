#pragma once
#include "c_instance_list.h"

#define SAVE_SLOT_COUNT 20

struct Map;
struct DepotManager;
struct InstanceManager;
struct InstanceSlot;

struct SaveFile{
	// Indexed by map of size mapcount(engine settings). -1 means "no save for
	// this map in this slot yet, spawn fresh from the prototype depot".
	int *item_indexs;
};

struct SaveManager{
	int slot_cursor;
	struct SaveFile *slot[SAVE_SLOT_COUNT];
};

struct SaveManager *si_create_save_manager();
void si_free_save_manager(struct SaveManager *sm);
void si_update_save_cursor(struct SaveManager *g_save_manager, int new_cursor);
void si_free_save_file(struct SaveFile *sf);

// Returns the save-item index active for map_gindx under the current slot
// cursor, or -1 if none. -1 means the caller should spawn defaults.
int si_get_active_item_index(struct SaveManager *sm, int map_gindx);

// Builds "<SAVE_BASE_DIR>/<map_gindx>_map/<item_gindx>_<INST_TYPE>.save".
// Returns NULL (and allocates nothing) if there's no active save for this
// map/slot. Caller owns and must free() the returned string.
char *si_resolve_instance_path(struct SaveManager *sm, int map_gindx, enum InstanceType type);

// The save-manager <-> stage-loader connection point: populates both instance
// managers for a freshly-loaded map, either from a save file (if the active
// slot has one for this map) or by spawning fresh instances from islots +
// the (immutable) prototype depot via depot_index.
void si_load_stage_instances(struct SaveManager *sm, int map_gindx, struct DepotManager *dman,
	struct InstanceManager *entities, struct InstanceSlot *entity_slots, int entity_slot_count, int entity_depot_index,
	struct InstanceManager *interactables, struct InstanceSlot *interactable_slots, int interactable_slot_count, int interactable_depot_index);

// Writes both instance managers out as a brand-new save item for map_gindx.
// Doesn't point any slot at it - call si_set_to_slot afterward if you want
// the active slot to pick it up.
void si_save_map_instances(struct SaveManager *sm, int map_gindx, struct InstanceManager *entities, struct InstanceManager *interactables);

// Loads a specific save item (not necessarily the active slot's) straight
// into already-created instance managers, e.g. for a load-game menu.
void si_load_map_instances(struct SaveManager *sm, int map_gindx, int item_gindx, struct InstanceManager *entities, struct InstanceManager *interactables);

void si_set_to_slot(struct SaveManager *sm, int map_gindx, int item_gindx, int save_slot);
void si_clear_to_slot(struct SaveManager *sm, int map_gindx, int save_slot);
