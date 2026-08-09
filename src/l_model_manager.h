#pragma once
#include <raylib.h>

struct LoadedMapModel{
	Model model;
	BoundingBox bounds;
};

void l_load_map_model(int gindx);
void l_draw_map_model(Camera cam);
