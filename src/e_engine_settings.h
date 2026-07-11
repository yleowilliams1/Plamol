#pragma once
#include <stdbool.h>
struct SettingsBind{
	int type;
	int action;
	int key;
};
enum EngStrings{
	MAP_PATH,
	TILESET_PATH,
	TILE_PATH,
	PORTRAIT_PATH,
	FLAG_PATH,
	ITEMS_PATH,
	STATS_PATH,
	INVENTORY_PATH,
	INPUT_PATH,
	ENG_STR_COUNT,
};
struct EngineSettings{
	char *strings[ENG_STR_COUNT];
};
float e_get_tilemem_secs();
void e_free_setting();
bool e_load_engine_settings();
