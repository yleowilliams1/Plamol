#pragma once
#include <stdint.h>
#include <stdbool.h>

// Think about not making sprite a dou
#include <raylib.h>

#include "t_pool.h"
#include "t_math.h"

#include "c_data_enums.h"
#include "c_flag_enums.h"

struct DouChildInvSlot{
	int item_gindx;
	int count;
	bool filled;
};

struct DouEntityPrototype{
	struct PoolHeader header;
	
	int entity_data[ENTITY_DATA_COUNT];
	int bstats_data[BASE_STAT_COUNT];

	int inventory_cap;
	int hotbar_cap;

	struct DouChildInvSlot *inventory;
	struct DouChildInvSlot *hotbar;

	uint32_t flags;
};

struct DouInteractablePrototype{
	struct PoolHeader header;	
	
	int interactable_data[INTERACTABLE_DATA_COUNT];
	enum InterTypeFlag type;
	uint32_t flags;
};


struct DouChildItemMod{
	union{
		int bstat;
		int dstat;
	};
	int amount;

};
struct DouItemPrototype{
	struct PoolHeader header;

	struct DouChildItemMod modifer[ITEM_MOD_DATA_COUNT];
	char *strings[ITEM_STR_DATA_COUNT];
	
	int range;

	uint32_t flags;	
};
struct DouSpritePrototype{
	struct PoolHeader header;

	char *sprite_path;
	
	int animation_count;
	int *frame_count;

	v2 **origin;

	Texture2D texture;
};
