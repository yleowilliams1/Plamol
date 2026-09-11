#pragma once
#include <raylib.h>

struct Editor{
	Camera2D cam;
};

struct Editor *init_editor();
void update_editor(struct Editor *editor);
void draw_editor(struct Editor *editor);
void free_editor(struct Editor *editor);
