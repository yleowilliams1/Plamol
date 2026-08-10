#pragma once
#include <stdbool.h>
struct local_indx;
struct config_pack{
	char line[256];
	char current_section[64];
	char key[64];
	char value[128];
};
typedef void (*ParserType)(struct config_pack, void *ptr);

bool t_loader(int gindx, struct local_indx *iman, ParserType func, char *path, void *ptr, int lindx);
char *t_ini_plus_indx(char *base, int indx);
char *t_png_plus_indx(char *base, int indx);
bool t_check(char *line, char *arg);
bool t_config(void *ptr, char *path, ParserType func);
