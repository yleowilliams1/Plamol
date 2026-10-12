#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "util/util.h"
struct RuntimeComponent;
struct Map;
void update_move(struct RuntimeComponent *m, int32_t *tx,int32_t *ty, int8_t *dir);
void move(struct RuntimeComponent *m,struct Map *map, int32_t *tx, int32_t *ty, int8_t *dir, v2 end);
bool get_move_offset(struct RuntimeComponent *m, vf2 *out);
bool is_moving(struct RuntimeComponent *m);

