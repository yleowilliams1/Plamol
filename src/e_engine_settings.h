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
	struct SettingsBind *default_binds_list;
	int default_binds_count;
	struct SettingsBind *override_binds_list;
	int override_binds_count;
};
char *e_get_path_to_tilesets();
char *e_get_map_path();
void e_free_setting();
bool e_load_engine_settings();
