#pragma once
#include "entity.h"
#include "map.h"
struct EntityManager; 
struct SpriteManager;

struct GameData{
	struct Map *m;
};

struct GameData create_game();
void start_game(
void update_game(struct EntityManager *e, struct SpriteManager *s);
