#pragma once
#include <stdint.h>
#include <stdbool.h>

#include "t_pool.h"
#include "t_math.h"

#include "c_magic_number.h"
#include "c_data_enums.h"
#include "c_flag_enums.h"

struct DouManager;

struct InstanceChildAnimState{
	float elapsed_time;
	int current_frame;
	int current_animation;
};
struct InstanceChildInvSlot{
	int item_gindx;
	int count;
	bool filled;
};
struct EntityInstance{
	struct PoolHeader h;

	int proto_gindx;
	int guid;
	v3 tile;
	enum TileDirFlag facing;

	struct InstanceChildAnimState anim;
	uint32_t runtime_flags;
	
	int current_hp;
	int current_ap;
	
	int bstats[BASE_STAT_COUNT];

	struct InstanceChildInvSlot inventory[MAX_INV_SIZE];
	struct InstanceChildInvSlot hotbar[MAX_HOTBAR_SIZE];
};
void inst_anim_advance(struct InstanceChildAnimState *anim, struct DouManager *dou, int gindx, float delta);
int inst_derive_stat(struct EntityInstance *entity, int stat, enum StatType type);
