#pragma once

struct Editor{
	int a;
};

struct Editor *init_editor();
void update_editor(struct Editor *editor);
void draw_editor(struct Editor *editor);
void free_editor(struct Editor *editor);
