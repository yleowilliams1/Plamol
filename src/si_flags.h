#pragma once
#include "magic_numbers.h"
#include <stdbool.h>
#include <stdint.h>


struct Flag{
	uint32_t hash;
	bool     value;
};

struct FlagManager{
	struct Flag flags[MAX_FLAGS];
	int count;
};

void f_init_flag();
void f_free_flag();
bool flag_get(const char *name);
void flag_set(const char *name, bool value);
