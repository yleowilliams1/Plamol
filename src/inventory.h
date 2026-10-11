#pragma once
#include <stdint.h>
#include "stats.h"
#define HOTBAR_SIZE 8
struct InventoryBonusMatrix{
	int stats[ST_STAT_COUNT];
	int skills[SK_SKILL_COUNT];
	int derived[DE_DERIVED_COUNT];
};
struct ItemPrototype{
	int32_t uid;
	int32_t value;
	struct InventoryBonusMatrix matrix; 
	int min_dmg;
	int max_dmg;
	enum SkillsEnum attack_skill_use;
	int weight;
	int name_str;
	int description_str;
	int max_stack;
};
struct ItemStack{
	int32_t uid;
	int32_t count;
};

struct Inventory{
	struct ItemStack hotbar_items[HOTBAR_SIZE];
	int16_t item_stack_count;
	struct ItemStack *item_stack;
	
};

