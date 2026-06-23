#pragma once
#include <stdbool.h>
struct Entity{
	int x;
	int y;
	int stats;
};

struct Entity *p_init_entity(int tx, int ty, int stat);
void p_free(struct Entity *e);
void p_move_x(struct Entity *e, bool can_move, int dir);
void p_move_y(struct Entity *e, bool can_move, int dir);
