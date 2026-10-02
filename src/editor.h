#pragma once
#include <raylib.h>

#define TOP_BUTTONS\
	X(NEW_MAP)\
	X(SAVE_MAP)\
	X(LOAD_MAP)\
	X(HIDE_ENTITY)\
	X(HIDE_SPRITES)\
	X(HIDE_EXITS)
#define BOTTOM_BUTTONS\
	X(SPRITE_LAYER)\
	X(ENTITY_LAYER)\
	X(EXIT_LAYER)\
	X(FLAG_LAYER)\
	X(MAP_PROPERTIES)

enum TopButtons{
	#define X(name) name,
	TOP_BUTTONS
	#undef X
	TOP_BUTTON_COUNT
};
enum ActiveData{
	#define X(name) name,
	BOTTOM_BUTTONS
	#undef X
	ACTIVE_DATA_COUNT
};

struct Editor{
	FilePathList available_sprites;
	FilePathList available_entities;
	FilePathList available_items;

	enum ActiveData active_data;
	Rectangle top;
	Rectangle bottom;
	Rectangle right;
	Rectangle left;
	Rectangle map_frame;
	
	float top_delta;
	float bottom_delta;
};
static const char *top_labels[] = {
	#define X(name) #name,
	TOP_BUTTONS
	#undef X
};
static const char *bottom_labels[] = {
	#define X(name) #name,
	BOTTOM_BUTTONS
	#undef X
};
struct Editor *init_editor();
void update_editor(struct Editor *editor);
void draw_editor(struct Editor *e);
void free_editor(struct Editor *editor);
