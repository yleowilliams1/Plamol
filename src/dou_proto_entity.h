#pragma once
#include <stdbool.h>
#include <stdint.h>

enum EntityFlags{
	ENT_COMBAT,
	ENT_SPRITE,
	ENT_TEXT,
	ENT_LOOT,
	ENT_PLAYER,
	ENT_HOSTILE,
	ENT_STAT,
	ENT_INV,
	ENTITY_FLAG_COUNT,
};

enum EntityComponents{
	E_SPRITE,
	E_TEXT,
	E_LOOT,
	E_POSX,
	E_POSY,
	E_STAT,
	E_INV,
	ENTCOM_COUNT,
};

struct Entity{
	int data[ENTCOM_COUNT];
	uint32_t flags;
};

struct DouLoader;

struct DouLoader dou_entity();
