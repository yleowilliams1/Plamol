#pragma once
#include <stdbool.h>
#include <stdint.h>


struct Flag{
	uint32_t hash;
	bool     value;
};

struct FlagManager{
	struct Flag *flags;
	int cap;
	int count;
};

struct FlagManager *si_init_flag();
bool si_free_flag(struct FlagManager *fm);
bool si_flag_get(const char *name, struct FlagManager *fm);
void si_flag_set(const char *name, bool value, struct FlagManager *fm);
