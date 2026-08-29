#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "t_math.h"
#include "t_instance_manager.h"
#include "depo_sprite.h"

#include "c_stat_list.h"

#define FLAG_LIST\
   	X(EF_CAN_COMBAT) \
   	X(EF_HAS_SPRITE) \
   	X(EF_HAS_INV) \
	X(EF_HAS_STAT) \
	X(EF_IS_HOSTILE) \
	X(EF_IS_DEAD) \
    	X(EF_DOES_COLLIDE) \
	X(EF_ON_TILE) 	

enum EntityFlags{
	#define X(name) name,
	FLAG_LIST
	#undef X
	ENTITY_FLAG_COUNT,
};

struct ChildInventorySlot{
	int item_gindx;
	int count;
	bool filled;
};
struct EntityPrototype{
	int bstat_data[BASE_STAT_COUNT];
	
	int sprite_gindx;
	
	int hp;
	int ap;

	int inventory_cap;
	int hotbar_cap;

	struct ChildInventorySlot *inventory;
	struct ChildInventorySlot *hotbar;

	uint32_t flags;
};

struct ItemFunctions;

struct ItemFunctions entity_prototype();
struct InstanceFunctions entity_instance();

const char *entflgstr(enum EntityFlags flag);
const char *bstatstr(enum BaseStatEnum stat);
const char *dstatstr(enum DerivedStatEnum stat);
