#pragma once
#include <raylib.h>
#include <stdbool.h>

struct Map;
struct EntityManager;

enum InteractState{
	IS_IDLE,
	IN_DIALOGUE,
	MID_WALK,
};

struct InteractManager{
	enum InteractState istate;
	bool is_combat;
};

struct InteractManager *create_interact();
// This gets called when you click
void update_interact(struct InteractManager *t, struct EntityManager *e, struct Map *m, Vector2 mouse_world_pos);
void free_interact(struct InteractManager *m);
void set_combat_interact(struct InteractManager *m, bool flag);
