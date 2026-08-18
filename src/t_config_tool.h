#pragma once
#include "t_gindex_tool.h"
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
struct DouLoadData{
	int gindx;
	int cap;
	struct local_indx *iman;
	void *iarr;
	size_t element_size;
	char *path;
	char *format;
	Loader loader;
	Init init;
	PostLoad pload;
};
struct DouFreeData{
	int *lindx;
	int *gindx;
	int cap;

	struct local_indx *iman;
	void *iarr;

	size_t element_size;
	Freer freer;	
};

char *t_format_path(char *base, char *format, int num);
bool t_check(char *line, char *arg);
bool t_config(void *ptr, char *path, Loader func);
bool t_handle_dou_loading(struct DouLoadData data);
bool t_handle_dou_freeing(struct DouFreeData data);
