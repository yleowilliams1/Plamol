#pragma once
#include <stdbool.h>
#include "p_entity.h"
#include "i_inventory.h"
#include "p_stats.h"
#include "t_math.h"

struct LootData;

struct EntityInstance{
	int entity_gindx;
	int GUID;
	bool valid;
	bool dirty;
	struct Entity e;
	struct Inventory i;
	struct BaseStats s;
};

void e_draw_entity_pool(struct EntityInstance *pool, int size);
bool e_consume_item(struct EntityInstance *e);
bool e_move_entity(struct EntityInstance *e, vf2 norm_input, float speed, float delta, bool can_move);
bool e_mod_health(struct EntityInstance *e, int amount);
// Never modify dstat directly.
bool e_grab_dstat(struct EntityInstance *e, enum Dev dstat, int *out);
bool e_grab_bstat(struct EntityInstance *e, enum Stats bstat, int *out);
bool e_grab_loot(struct EntityInstance *e, struct LootData *out);
