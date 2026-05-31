#pragma once
#include <stdbool.h>
struct SettingsBind{
	int type;
	int action;
	int key;
};

struct EngineSettings{
	char *map_path;
	struct SettingsBind *default_binds_list;
	int default_binds_count;
	struct SettingsBind *override_binds_list;
	int override_binds_count;
};

char *e_get_map_path();
void e_free_setting();
bool e_load_engine_settings();
