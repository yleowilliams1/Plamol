#include "t_log_handler.h"
#include "c_flag_enums.h"

char *entity_flag_str(enum EntityFlag value){
	char *result = NULL;
	if(value < 0 || value >= ENTITY_FLAG_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case EF_CAN_COMBAT: result = "Can Combat"; break;
		case EF_HAS_SPRITE: result = "Has Sprite"; break;
		case EF_HAS_INV: result = "Has Inventory"; break;
		case EF_HAS_STAT: result = "Has Stat"; break;
		case EF_IS_HOSTILE: result = "Is Hostile"; break;
		case EF_IS_DEAD: result = "Is Dead"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *tiledir_flag_str(enum TileDirFlag value){
	char *result = NULL;
	if(value < 0 || value >= TILE_DIRECTION_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case ED_NORTH: result = "North"; break;
		case ED_SOUTH: result = "South"; break;
		case ED_EAST: result = "East"; break;
		case ED_WEST: result = "West"; break;
		case ED_NORTH_WEST: result = "North West"; break;
		case ED_NORTH_EAST: result = "North East"; break;
		case ED_SOUTH_WEST: result = "South West"; break;
		case ED_SOUTH_EAST: result = "South East"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *inter_flag_str(enum InterFlag value){
	char *result = NULL;
	if(value < 0 || value >= INTERACTABLE_FLAG_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case ET_IS_OPEN: result = "Is Open"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *intertype_flag_str(enum InterTypeFlag value){
	char *result = NULL;
	if(value < 0 || value >= INTERACTABLE_TYPE_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case ETT_IS_DOOR: result = "Is Door"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *item_flag_str(enum ItemFlag value){
	char *result = NULL;
	if(value < 0 || value >= ITEM_FLAG_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case EI_IS_THROWABLE: result = "Is Throwable"; break;
		case EI_IS_CONSUMEABLE: result = "Is Consumeable"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *dou_flag_str(enum DouFlag value){
	char *result = NULL;
	if(value < 0 || value >= DOU_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case EOU_ITEM: result = "Item"; break;
		case EOU_ENTITY: result = "Entity"; break;
		case EOU_SPRITE: result = "Sprite"; break;
		case EOU_INTERACTABLE: result = "Interactable"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *si_flag_str(enum SiFlag value){
	char *result = NULL;
	if(value < 0 || value >= SI_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case ESI_FLAGS: result = "Flags"; break;
		case ESI_INPUT: result = "Input"; break;
		case ESI_MAP: result = "Map"; break;
		case ESI_STATEMACHINE: result = "State Machine"; break;
		case ESI_WORLD: result = "World"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *instance_flag_str(enum InstanceFlag value){
	char *result = NULL;
	if(value < 0 || value >= INSTANCE_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case EIN_ENTITY: result = "Entity"; break;
		case EIN_INTERACTABLE: result = "Interactable"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
