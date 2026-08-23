#pragma once
#include "t_math.h"
#include <raylib.h>

struct SpritePrototype{
	char *sprite_path;
	int animation_count;
	int *frame_count;
	v2 **origin;
	Texture2D texture;
};

struct ItemFunctions;
struct ItemFunctions sprite_prototype();
