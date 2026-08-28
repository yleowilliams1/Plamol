#pragma once

#define SAVE_SLOT_COUNT 20

struct Map;
struct DepotManager;

struct SaveFile{
	// Indexed by map of size mapcount(engine settings)
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

void si_save_map_depot(struct SaveManager *sm, int map_gindx, struct DepotManager *dman);
void si_load_map_depot(struct SaveManager *sm, int map_gindx, int item_gindx, struct DepotManager *dman);
void si_set_to_slot(struct SaveManager *sm, int map_gindx, int item_gindx, int save_slot);
void si_clear_to_slot(struct SaveManager *sm, int map_gindx, int save_slot);
