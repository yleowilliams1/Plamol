#pragma once
#include <stdbool.h>
#include <raylib.h>
#include "t_math.h"
#define MAX_ANIMATIONS 128
#define MAX_FRAMES 128

struct SpriteMetadata{
	int sprite_gindx;
	bool animated;
	float time_per_frame;
	int animations_y;
	int frame_x[MAX_ANIMATIONS];
	v2 origin[MAX_ANIMATIONS][MAX_FRAMES];
};
struct SpriteData{
	struct SpriteMetadata metadata;
	Texture2D texture;
};	

bool l_free_sprite(int gindx);
bool l_load_sprite(int gindx);
void l_draw_sprite(int gindx, bool autoload, Vector2 position, int animation, int frame);
