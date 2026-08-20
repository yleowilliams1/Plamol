#pragma once
#include <stdbool.h>
#include <stdio.h>

struct config_pack{
	char line[256];
	char current_section[64];
	char key[64];
	char value[128];
};

typedef void (*Loader)(struct config_pack, void *ptr);
typedef void (*Freer)(void *slot);
typedef void (*Init)(void *slot);
typedef void (*PostLoad)(void *slot);

char *t_format_path(char *base, char *format, int num);
bool t_check(char *line, char *arg);
bool t_config(void *ptr, char *path, Loader func);
