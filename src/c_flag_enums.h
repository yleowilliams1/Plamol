#pragma once

enum EntityFlag{
	EF_CAN_COMBAT,
	EF_HAS_SPRITE,
	EF_HAS_INV,
	EF_HAS_STAT,
	EF_IS_HOSTILE,
	EF_IS_DEAD,
	ENTITY_FLAG_COUNT,
};
enum TileDirFlag{
	ED_NORTH,
	ED_SOUTH,
	ED_EAST,
	ED_WEST,
	ED_NORTH_WEST,
	ED_NORTH_EAST,
	ED_SOUTH_WEST,
	ED_SOUTH_EAST,
	TILE_DIRECTION_COUNT,
};
enum InterFlag{
	ET_IS_OPEN,
	INTERACTABLE_FLAG_COUNT,
};
enum InterTypeFlag{
	ETT_IS_DOOR,
	INTERACTABLE_TYPE_COUNT,
};
enum ItemFlag{
	EI_IS_THROWABLE,
	EI_IS_CONSUMEABLE,
	ITEM_FLAG_COUNT,
};
enum DouFlag{
	EOU_ITEM,
	EOU_ENTITY,
	EOU_SPRITE,
	EOU_INTERACTABLE,
	DOU_COUNT,
};
enum SiFlag{
	ESI_FLAGS,
	ESI_INPUT,
	ESI_MAP,
	ESI_STATEMACHINE,
	ESI_WORLD,
	SI_COUNT,
};
enum InstanceFlag{
	EIN_ENTITY,
	EIN_INTERACTABLE,
	INSTANCE_COUNT,
};
char *entity_flag_str(enum EntityFlag value);
char *tiledir_flag_str(enum TileDirFlag value);
char *inter_flag_str(enum InterFlag value);
char *intertype_flag_str(enum InterTypeFlag value);
char *item_flag_str(enum ItemFlag value);
char *dou_flag_str(enum DouFlag value);
char *si_flag_str(enum SiFlag value);
char *instance_flag_str(enum InstanceFlag value);
