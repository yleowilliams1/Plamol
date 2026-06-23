#pragma once
#include <stdbool.h>
struct SettingsBind{
	int type;
	int action;
	int key;
};

struct EngineSettings{
	char *map_path;
	char *path_to_tilesets;
	char *tile_path;
	char *portrait_path;
	char *flag_path;
	char *items_path;
	char *stats_path;
	struct SettingsBind *default_binds_list;
	int default_binds_count;
	struct SettingsBind *override_binds_list;
	int override_binds_count;
};
char *e_get_path_to_tilesets();
char *e_get_map_path();
char *e_get_tile_path();
char *e_get_portrait_path();
char *e_get_flag_path();
char *e_get_items_path();
char *e_get_stats_path();
void e_free_setting();
bool e_load_engine_settings();
