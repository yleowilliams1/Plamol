#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#define MAX_FLAG_NAME 32

struct Flag{
	char name[MAX_FLAG_NAME];
	uint32_t hash;
	uint8_t value;
};

struct FlagManager{
	struct Flag *flags;
	uint32_t count;
	uint32_t capacity;
};

void flag_set(struct FlagManager *fm, const char *name, bool value);
bool flag_get(struct FlagManager *fm, const char *name);
void flag_free(struct FlagManager *fm);
void flip_flag(struct FlagManager *fm);
