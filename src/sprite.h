#pragma once
#include <raylib.h>
#include "util/util.h"

struct Map; // fwd-declared - only sprite.c needs the real definition

// This one gets loaded and never touched again - same contract as EntityImmutable.
struct SpriteData{
	char *sprite_path;
	int animation_count;
	int *frame_count;
	v2 **origin;
	Texture2D texture;
	int gindex;

	// Pixel size of a single frame in the sheet. Frames are laid out in a
	// grid: animation_count rows, frame_count[anim] columns per row. A
	// frame's source rect is therefore (frame_index*frame_w, anim_index*frame_h,
	// frame_w, frame_h). Derived at load time in load_sprite_from_disk from
	// the texture's actual pixel size - not read from the config.
	int frame_w;
	int frame_h;
};
struct SpriteManager{
	// Same style as EntityManager: gindex-indexed, NULL when unloaded.
	struct SpriteData **sprite;
};

struct SpriteManager *create_sprite_manager();
void free_sprite_manager(struct SpriteManager **sman);
void load_map_sprites(struct SpriteManager *sman, struct Map *map);
void unload_map_sprites(struct SpriteManager *sman, struct Map *map);
struct SpriteData *load_sprite_from_disk(int sprite_gindex);
void free_sprite_at(struct SpriteData **s, int gindedx);
