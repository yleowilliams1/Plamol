#pragma once
#include <stdint.h>
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
struct InventoryChildInstance{
	int gindx[INVENTORY_SIZE];
	int count[INVENTORY_SIZE];
	int hotbar[HOTBAR_SIZE];
};
struct StatChildInstance{
	int base[BASE_STAT_COUNT];
	int mod[BASE_STAT_COUNT];
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

	struct StatChildInstance stats;
	struct InventoryChildInstance inv;
};
struct InteractableInstance{
	struct PoolHeader h;
	
	int proto_gindx;
	int guid;
	v3 tile;

	uint32_t runtime_flags;

	struct InstanceChildAnimState anim;
	struct InventoryChildInstance inv;
};

void inst_anim_advance(struct InstanceChildAnimState *anim, struct DouManager *dou, int gindx, float delta);
int inst_derive_stat(struct StatChildInstance *stats, enum Dev type);
