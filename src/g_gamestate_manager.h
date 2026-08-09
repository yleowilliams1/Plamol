#pragma once
#include <raylib.h>
#include "m_map.h"

struct GameState{
	int gindx_strtmap;
	struct MapData map;
	Camera cam;
};

void g_load_gamestate();
void g_update_gamestate();
Camera *grab_cam();
void g_draw_gamestate();

