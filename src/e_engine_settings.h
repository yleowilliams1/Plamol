#pragma once
#include <stdbool.h>
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

char *e_grab_str(enum EngStrings type);
void e_free_setting();
bool e_load_engine_settings();
