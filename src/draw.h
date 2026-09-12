#pragma once
#include <raylib.h>
struct DrawItem{
	Texture2D texture;
	Rectangle source; // which frame of the sheet to draw - see draw.c
	Vector2 position;
	float depth;
};

struct EntityManager;
struct SpriteManager;
struct Map;
void draw_map(struct EntityManager *entity_manager, struct Map *map, struct SpriteManager *sprite_manager);
