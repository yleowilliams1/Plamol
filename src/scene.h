#pragma once
#include <raylib.h>
#include "entity.h"
#include "map.h"
#include "save.h"
#include "sprite.h"
struct Scene{
	struct EntityManager *entity_manager;
	struct SpriteManager *sprites_manager;
	struct SaveManager *save_manager;
	struct Map *map;
};

struct Scene *init_scene();
void update_scene(struct Scene *scene);
void draw_scene(struct Scene *scene);
void free_scene(struct Scene *scene);
