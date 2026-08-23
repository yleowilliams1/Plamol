#pragma once
#include <stdbool.h>
#include <stdio.h>

struct config_pack{
	char line[256];
	char current_section[64];
	char key[64];
	char value[128];
};

typedef void (*ConfigLoader)(struct config_pack, void *ptr);

char *t_format_path(char *base, char *format, int num);
bool t_check(char *line, char *arg);
bool t_config(void *ptr, char *path, ConfigLoader func);
