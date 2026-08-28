#pragma once
#include <stdbool.h>

#include "t_math.h"
#include "t_depot_manager.h"

#include "depo_sprite.h"


typedef void (*Update)(void *slot, float delta);
typedef void (*Draw)(void *slot, float delta);
typedef void (*Interact)(void *slot, void *message);

struct InstanceFunctions{
	Update on_update;
	Draw on_draw;
	Interact on_interact;
};
struct Instance{
	void *prototype_copy;
	
	bool is_global_coordinates;
	bool can_passthrough;

	v3 pos;
	
	struct AnimationState anim;
	
	enum Direction facing;
};
struct InstanceManager{
	struct Instance **instances;
	struct InstanceFunctions fncs;
	int count;
};

struct InstanceManager *t_create_instance_manager(struct InstanceFunctions fncs, int count);
void t_free_instance_manager(struct InstanceManager *instance_manager);
