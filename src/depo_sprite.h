#pragma once
#include "t_math.h"
#include <raylib.h>
#define DIR_LIST \
	X(D_NORTH) \
	X(D_SOUTH) \
	X(D_EAST) \
	X(D_WEST) \
	X(D_NORTH_WEST) \
	X(D_NORTH_EAST) \
	X(D_SOUTH_WEST) \
	X(D_SOUTH_EAST) 
enum Direction{
	#define X(name) name,
	DIR_LIST
	#undef X
	DIRECTION_COUNT
};
struct AnimationState{
	float elapsed_time;
	int current_frame;
	int current_animation;
};
struct SpriteData{
	char *sprite_path;
	int animation_count;
	int *frame_count;
	v2 **origin;
	Texture2D texture;
};

struct ItemFunctions;
struct ItemFunctions sprite_data();
