#pragma once
#include <raylib.h>
struct MapPack;

// Tile grid -> world space. Exposed so entities and anything else on the grid
// project through the same function the map does instead of duplicating it.
Vector2 m_tile_to_world(int tile_x, int tile_y, int tile_z);
void m_draw_map(struct MapPack *m, int player_z);
