#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "util/util.h"
#include "settings.h"
#include "editor.h"

#define PANEL_HEIGHT 75
#define SIDE_WIDTH 128
static void top_fnc(struct Editor *e, int indx);
static void update_recs(struct Editor *e);
struct Editor *init_editor(){
	struct Editor *e = XCALLOC(1, sizeof(struct Editor));
	e->available_sprites = LoadDirectoryFiles(STR(SPRITE_PATH));
	e->available_entities = LoadDirectoryFiles(STR(ENTITY_PATH));
	e->available_items = LoadDirectoryFiles(STR(ITEM_PATH));	
	update_recs(e);	
	return e;
}
void update_editor(struct Editor *editor){
	if(!editor){LOG(IS_NULL, "Editor is NULL"); return;}
	if(IsWindowResized()){update_recs(editor);}
	if(CheckCollisionPointRec(GetMousePosition(), editor->map_frame)){
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){LOG(LOAD, "Is on frame");}
	}
}
void draw_editor(struct Editor *e){
	if(!e){LOG(IS_NULL, "Editor is NULL"); return;}
	BeginDrawing();
	ClearBackground(BLACK);
	GuiPanel(e->top, NULL);
	for(int i = 0; i < TOP_BUTTON_COUNT; i++){
		Rectangle r = {i * e->top_delta, e->top.y, e->top_delta, PANEL_HEIGHT};
		if(GuiButton(r, top_labels[i])){top_fnc(e, i);}	
	}
	GuiPanel(e->bottom, NULL);
	for(int i = 0; i < ACTIVE_DATA_COUNT; i++){
		Rectangle cell = {i * e->bottom_delta, e->bottom.y, e->bottom_delta, PANEL_HEIGHT};
		float box = 20;
		Rectangle cb = {cell.x + 10, cell.y + (cell.height - box) / 2, box, box};

		bool on = (e->active_data == i);
		GuiCheckBox(cb, bottom_labels[i], &on);
		if(on) e->active_data = i;
	}
	GuiPanel(e->left, NULL);

	GuiPanel(e->right, NULL);
	EndDrawing();
}
void free_editor(struct Editor *editor){
	if(!editor){LOG(IS_NULL, "Editor is NULL"); return;}
	UnloadDirectoryFiles(editor->available_sprites);
	UnloadDirectoryFiles(editor->available_entities);
	UnloadDirectoryFiles(editor->available_items);
	free(editor);
}

static void top_fnc(struct Editor *e, int indx){
	return;
}
static void update_recs(struct Editor *e){
	if(!e){LOG(IS_NULL, "Editor is NULL"); return;}	
	int sw = GetScreenWidth();
	int sh = GetScreenHeight();
	e->top = (Rectangle){0, 0, sw, PANEL_HEIGHT};
	e->bottom = (Rectangle){0,  sh - PANEL_HEIGHT, sw,  PANEL_HEIGHT};
	
	e->right = (Rectangle){sw - SIDE_WIDTH, PANEL_HEIGHT, SIDE_WIDTH, sh - (PANEL_HEIGHT * 2)};
	e->left = (Rectangle){0, PANEL_HEIGHT, SIDE_WIDTH, sh - (PANEL_HEIGHT * 2)};
	
	e->top_delta    = (float)sw / TOP_BUTTON_COUNT;
	e->bottom_delta = (float)sw / ACTIVE_DATA_COUNT;
	
	e->map_frame = (Rectangle){SIDE_WIDTH, PANEL_HEIGHT, sw - (SIDE_WIDTH * 2), sh - (PANEL_HEIGHT * 2)};	
}
