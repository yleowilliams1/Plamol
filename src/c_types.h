#pragma once
#include <stdint.h>
#include "c_magic_number.h"

enum Stats{
	STR,
	DEX,
	CON,
	SOC,
	INT,
	WIS,
	BSTAT_COUNT,
};

enum Dev{
	PHYS_CORD,
	WORD,
	PROB_ANALYSIS,
	SPATIAL,
	MUSICAL,
	NATURAL,
	INTERPERSONAL,
	INTRAPERSONAL,
	INNOCENCE,
	HEROISM,
	LOVE,
	AUTHORITARIAN,
	AC,
	DR,
	MAX_AP,
	MAX_HP,
	INITATIVE,
	DERV_CAP,
};

struct BaseStats{
	int basestats[BSTAT_COUNT];
};
enum TileDirections{
	W_NORTH,
	W_SOUTH,
	W_EAST,
	W_WEST,
	W_NORTH_EAST,
	W_SOUTH_WEST,
	W_NORTH_WEST,
	W_SOUTH_EAST,
	W_DIR_COUNT,
};

enum EntityFlags{
	ENT_COMBAT,
	ENT_SPRITE,
	ENT_TEXT,
	ENT_LOOT,
	ENT_PLAYER,
	ENT_HOSTILE,
	ENT_STAT,
	ENT_INV,
	ENT_DEAD,
	ENTITY_FLAG_COUNT,
};
enum EntityComponents{
	E_SPRITE,
	E_TEXT,
	E_LOOT,
	E_STAT,
	E_INV,
	ENTCOM_COUNT,
};
struct Entity{
	int data[ENTCOM_COUNT];
	uint32_t flags;
};
struct Inventory{
	int gindx[INVENTORY_SIZE];
	int count[INVENTORY_SIZE];
	int hotbar[HOTBAR_SIZE];
};
