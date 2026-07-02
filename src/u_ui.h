#pragma once
#include <stdbool.h>
#include <raylib.h>
#define UI_LENGTH 64

enum TextType{
	TopLeft,
	TopRight,
	Top,
	Left,
	Right,
	BottomRight,
	BottomLeft,
	Bottom,
	Middle
};
struct UI_Element{
	Color colr;
	Rectangle col;
	Vector2 pos;
	
	char *text;
	int padding;
	enum TextType text_pos;
	bool valid;
	
	void (*button)(void);
	bool intersect;
};

void clear_ui_state();
int ui_element(enum TextType text_pos, Vector2 pos, Color clr, Rectangle collision, char *text, int padding, void(*button)(void));
void draw_ui();

