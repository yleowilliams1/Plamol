#pragma once
#include <stdio.h>
#define NULL_STR 0

struct GUIDs{
	int gen;
	size_t slot;
	size_t valid_slot;
};
struct StringTable{
	char *arr;
	size_t arr_size;
	size_t *offset;
	size_t string_count;
	
	int gen_count;
	struct GUIDs *valid_gens;
	int valid_gen_count;
};

struct StringTable *create_string_table();
void free_string_table(struct StringTable *tbl);
void pop_str(struct StringTable *tbl, int gen);
int stack_str(struct StringTable *tbl, char *str, size_t len);

