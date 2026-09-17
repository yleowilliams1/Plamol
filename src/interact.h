#pragma once
#include "util/util.h"
#include "entity.h"
#include <stdbool.h>
#define INTERACT_SIZE 128

struct InteractModule{
	float time;
	float opacity;		
	bool slot_active;
	struct EntityImmutable *immutable;
	struct EntityMutable *mutable;	
};

void update_interact();
void clear_interact();
void interact(struct EntityManager *entity, struct Map *map, vf2 world_position);
void draw_interact();
bool in_paused_interaction();
