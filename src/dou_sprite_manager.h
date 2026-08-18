#pragma once
#include <stdbool.h>
#include <raylib.h>
#include "t_math.h"

struct SpriteData{
	char *sprite_path;	

	int animation_count;
	int *frame_count; //index by current animation

	v2 **origin;// 2d heap array with x being current animation and y being current frame
	
	Texture2D texture;
};	

struct DouLoader;
struct DouLoader dou_sprite();
