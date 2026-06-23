#pragma once
#include <stdbool.h>
#include <stdint.h>

#define MAX_FLAGS 256
#define MAX_FLAG_NAME 32

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
