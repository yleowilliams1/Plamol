#pragma once
#include <stdbool.h>
#include "util/util.h"
void init_move();
void update_move();
void free_move();
// This needs to be called when entitiesa re cleared
void clear_move();
bool get_move_offset(int indx, vf2 *out);
bool is_moving(int indx);
void move(struct EntityMutable *e, struct Map *map, v2 end);
