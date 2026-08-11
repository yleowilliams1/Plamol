#pragma once
#include "m_map.h"
#include <raylib.h>

struct GameState{
	int gindx_strtmap;
	struct MapPack map;
	Camera2D cam;
	int player_z;
};

void g_load_gamestate();
void g_update_gamestate();
Camera2D *grab_cam();
void g_draw_gamestate();

