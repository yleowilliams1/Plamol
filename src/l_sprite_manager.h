#pragma once
#include <stdbool.h>
#include <raylib.h>
#include "t_math.h"
#define MAX_ANIMATIONS 128

struct SpriteMetadata{
	int sprite_gindx;
	bool animated;
	float time_per_frame;
	int animations_y;
	int frame_x[MAX_ANIMATIONS];
};
struct SpriteRuntime{
	float frame_count;
	int current_frame;
	int current_animation;
	Vector2 position;
	float rotation;
	Rectangle frame_rec;
};
struct SpriteData{
	struct SpriteMetadata metadata;
	struct SpriteRuntime runtime;
	Texture2D texture;
};	

bool l_free_sprite(int gindx);
bool l_load_sprite(int gindx);
void l_player_sprite(int gindx, int autoload);
void l_draw_sprite(int gindx, bool autoload, Vector2 position);
void l_reset_sprite(int gindx, int new_animation);
Vector2 *l_grab_sprite_pos(int gindx, bool autoload);
