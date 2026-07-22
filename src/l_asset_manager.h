#pragma once
#include <stdio.h>

#include "t_config_tool.h"

struct local_indx;

struct AssetLoadPackage{
	int gindx;
	struct local_indx *index_manager;
	int arr_cap;
	void *arr;
	size_t element_size;
	ParserType function;
	char *path;
};

struct AssetFreePackage{
	int gindx;
	struct local_indx *index_manager;
	int arr_cap;
	void *arr;
	size_t element_size;
};

bool l_load_asset(struct AssetLoadPackage pckg);
bool t_free_asset(struct AssetFreePackage pckg);
