#pragma once
#include <raylib.h>
struct MapPack;

struct EntityInstance;

void m_draw_map(struct MapPack *m, int player_z, struct EntityInstance *pool, int ent_size);
