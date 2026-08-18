#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct PoolHeader{
	uint32_t slot;
	uint32_t gen;
	bool active;
};
struct InRef{
	uint32_t slot;
	uint32_t gen;
};
struct Pool{
	void *items;
	size_t element_size;
	int cap;
	int count;
	uint32_t next_gen;
};

void *t_pool_alloc(struct Pool*p, struct InRef *out);
void *t_pool_get(struct Pool *p, struct InRef r);
bool t_pool_free(struct Pool *p, struct InRef r);
void *t_pool_next(struct Pool *p, struct InRef *out, int *cursor);
