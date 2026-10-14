#pragma once
#include "entity.h"
#include "map.h"
#include "string_table.h"
#include "save.h"

// General rule is this is just the living place of all this data. It never touches or changes it, all it does is create and destroy it.

struct GameLifetime{
	struct Map *m;
	struct StringTable *str;
	struct SaveManager *save;
	struct SpriteData *spr;
	struct EntityManager *en;
};

void create_gamelifetime();
void free_gamelifetime();
void update_gamelifetime();
void draw_gamelifetime();
