#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "t_math.h"

#include "c_flag_enums.h"

#define FLAG_LIST\
   	X(EF_CAN_COMBAT) \
   	X(EF_HAS_SPRITE) \
   	X(EF_HAS_INV) \
	X(EF_HAS_STAT) \
	X(EF_IS_HOSTILE) \
	X(EF_IS_DEAD) 

#define BSTAT_LIST \
	X(STRENGTH) \
	X(DEXTERITY) \
	X(CONSITUTION) \
	X(SOCIAL) \
	X(INTELLIGENCE) \
	X(WISDOM)

#define DSTAT_LIST \
	X(PHYSICAL_COORDIANTION) \
	X(WORD) \
	X(PROBLEM_ANALYSIS) \
	X(SPATIAL) \
	X(MUSICAL) \
	X(NATURAL) \
	X(INTERPERSONAL) \
	X(INTRAPERSONAL) \
	X(INNOCENCE) \
	X(HEROISM) \
	X(LOVE) \
	X(AUTHORITARIAN) \
	X(ARMOR_CLASS) \
	X(MAX_HEALTH_POINTS) \
	X(MAX_ACIONT_POINTS) \
	X(INITATIVE_BONUS) 

enum EntityFlags{
	#define X(name) name,
	FLAG_LIST
	#undef X
	ENTITY_FLAG_COUNT,
};
enum BaseStatEnum{
	#define X(name) name,
	BSTAT_LIST
	#undef X
	BASE_STAT_COUNT,
};
enum DerivedStatEnum{
	#define X(name) name,
	DSTAT_LIST
	#undef X
	DERIVED_STAT_COUNT,
};

struct ChildInventorySlot{
	int item_gindx;
	int count;
	bool filled;
};
struct EntityPrototype{
	int bstat_data[BASE_STAT_COUNT];
	
	int sprite_gindx;

	int inventory_cap;
	int hotbar_cap;

	struct ChildInventorySlot *inventory;
	struct ChildInventorySlot *hotbar;

	uint32_t flags;
};

struct AnimationState{
	float elapsed_time;
	int current_frame;
	int current_animation;
};
struct EntityInstanceLoadData{
	struct EntityPrototype *prototype;
	v3 start_tile;
	enum TileDirFlag start_direction;
};
struct EntityInstance{
	v3 tile;
	
	enum TileDirFlag facing;

	struct AnimationState anim;
	uint32_t runtime_flags;
	
	int current_hp;
	int current_ap;

	int bstat[BASE_STAT_COUNT];
	
	int inventory_size;
	int hotbar_size;

	struct ChildInventorySlot *inventory;
	struct ChildInventorySlot *hotbar;
};

struct ItemFunctions;

struct ItemFunctions entity_prototype();
struct ItemFunctions entity_instance();

const char *entflgstr(enum EntityFlags flag);
const char *bstatstr(enum BaseStatEnum stat);
const char *dstatstr(enum DerivedStatEnum stat);

