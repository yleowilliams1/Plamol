#pragma once

#define SAVE_SLOT_COUNT 20

struct Map;
struct DepotManager;

struct SaveFile{
	// This gets indexed into by the map.
	char **paths;	
};

struct SaveManager{
	int save_cursor;
	struct SaveFile *saves[SAVE_SLOT_COUNT];
};

struct SaveManager *si_create_save_manager();
void si_update_save_cursor(int new_cursor);

void si_save_from(int save, struct Map *map, struct DepotManager *dman);
void si_load_to(int save, struct Map *map, struct DepotManager *dman);
