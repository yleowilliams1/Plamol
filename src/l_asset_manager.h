#pragma once
#include <stdio.h>
#include <stdbool.h>

#include "t_config_tool.h"


typedef bool (*Loader)(int gindx);
typedef void (*Initializer)(void *slot);


struct local_indx;

struct AssetLoadPackage{
	int gindx;
	struct local_indx *index_manager;
	int arr_cap;
	void *arr;
	size_t element_size;
	ParserType function;
	char *path;
	Initializer init;
};

struct AssetFreePackage{
	int gindx;
	struct local_indx *index_manager;
	int arr_cap;
	void *arr;
	size_t element_size;
};
int l_getter_checks(int gindx, bool autoload, int cap, struct local_indx *iman, Loader ldr);
bool l_load_asset(struct AssetLoadPackage pckg);
bool t_free_asset(struct AssetFreePackage pckg);
