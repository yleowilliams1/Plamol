#pragma once
#include <stdbool.h>

struct config_pack{
	char line[256];
	char current_section[64];
	char key[64];
	char value[128];
};

bool t_check(char *line, char *arg);
bool t_config(void *ptr, char *path, void (*func)(struct config_pack, void *ptr));
