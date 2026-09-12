#pragma once
#include <raylib.h>
#include "map.h"
#include "sprite.h"
#include "entity.h"
#include "util/util.h"

enum EditorLayer{
	LAYER_NONE,
	LAYER_SPRITE,
	LAYER_ENTITY,
	LAYER_EXIT,
	LAYER_FLAG,
};

struct Editor{
	Camera2D cam;
	struct Map *map;
	struct SpriteManager *sprite;
	struct EntityManager *entity;

	enum EditorLayer current_layer;
	enum TileFlags paint_flag;
	struct Sprite paint_sprite;
	struct Entity paint_entity;
	struct ExitTile paint_exit;

	Vector2 mouse_world_pos;
	v2 mouse_tile_pos;
	vf2 mouse_tile_world_pos;
};

struct Editor *init_editor();
void update_editor(struct Editor *editor);
void draw_editor(struct Editor *editor);
void free_editor(struct Editor *editor);
