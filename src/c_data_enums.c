#include "t_log_handler.h"
#include "c_data_enums.h"

char *bstat_str(enum BStatsData value){
	char *result = NULL;	
	if(value < 0 || value >= BASE_STAT_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case SBDATA_STR: result = "Strength"; break;
		case SBDATA_DEX: result = "Dexterity"; break;
		case SBDATA_CON: result = "Constitution"; break;
		case SBDATA_SOC: result = "Social"; break;
		case SBDATA_INT: result = "Intelligence"; break;
		case SBDATA_WIS: result = "Wisdom"; break;
		default: result = NULL; break;	
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *dstat_str(enum DStatsData value){
	char *result = NULL;
	if(value < 0 || value >= DERIVED_STAT_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case SDDATA_PHYS_CORD: result = "Physical Coordination"; break;
		case SDDATA_WORD: result = "Word"; break;
		case SDDATA_PROB_ANALYSIS: result = "Problem Analysis"; break;
		case SDDATA_SPATIAL: result = "Spatial"; break;
		case SDDATA_MUSICAL: result = "Musical"; break;
		case SDDATA_NATURAL: result = "Natural"; break;
		case SDDATA_INTERPERSONAL: result = "Interpersonal"; break;
		case SDDATA_INTRAPERSONAL: result = "Intrapersonal"; break;
		case SDDATA_INNOCENCE: result = "Innocence"; break;
		case SDDATA_HEROISM: result = "Heroism"; break;
		case SDDATA_LOVE: result = "Love"; break;
		case SDDATA_AUTHORITARIAN: result = "Authoritarian"; break;
		case SDDATA_AC: result = "Armor Class"; break;
		case DDSDATA_DR: result = "Damage Reduction"; break;
		case SDDATA_MAX_AP: result = "Max Action Points"; break;
		case SDDATA_MAX_HP: result = "Max Health Points"; break;
		case SDDATA_INITATIVE_BONUS: result = "Initiative Bonus"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *edata_str(enum EntityData value){
	char *result = NULL;
	if(value < 0 || value >= ENTITY_DATA_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case EDATA_SPRITE: result = "Sprite"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *tdata_str(enum InteractableData value){
	char *result = NULL;
	if(value < 0 || value >= INTERACTABLE_DATA_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case TDATA_SPRITE: result = "Sprite"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *idata_str(enum ItemModiferData value){
	char *result = NULL;
	if(value < 0 || value >= ITEM_MOD_DATA_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case IDATA_ADD: result = "Add"; break;
		case IDATA_HIT: result = "Hit"; break;
		case IDATA_DAMAGE: result = "Damage"; break;
		case IDATA_CONSUME: result = "Consume"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
char *istrdata_str(enum ItemStrData value){
	char *result = NULL;
	if(value < 0 || value >= ITEM_STR_DATA_COUNT){
		LOG(LOG_NULL, "Passing out of range value of %d", value);
		return NULL;
	}
	switch(value){
		case ISTRDATA_NAME: result = "Name"; break;
		case ISTRDATA_DESCRIPTION: result = "Description"; break;
		default: result = NULL; break;
	}
	if(!result){LOG(LOG_NULL, "Passing NULL string value of %d", value);}
	return result;
}
