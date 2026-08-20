#pragma once

enum BStatsData{
	SBDATA_STR,
	SBDATA_DEX,
	SBDATA_CON,
	SBDATA_SOC,
	SBDATA_INT,
	SBDATA_WIS,
	BASE_STAT_COUNT,
};
enum DStatsData{
	SDDATA_PHYS_CORD,
	SDDATA_WORD,
	SDDATA_PROB_ANALYSIS,
	SDDATA_SPATIAL,
	SDDATA_MUSICAL,
	SDDATA_NATURAL,
	SDDATA_INTERPERSONAL,
	SDDATA_INTRAPERSONAL,
	SDDATA_INNOCENCE,
	SDDATA_HEROISM,
	SDDATA_LOVE,
	SDDATA_AUTHORITARIAN,
	SDDATA_AC,
	DDSDATA_DR,
	SDDATA_MAX_AP,
	SDDATA_MAX_HP,
	SDDATA_INITATIVE_BONUS,
	DERIVED_STAT_COUNT,
};
enum EntityData{
	EDATA_SPRITE,
	ENTITY_DATA_COUNT,
};
enum InteractableData{
	TDATA_SPRITE,
	INTERACTABLE_DATA_COUNT,
};
enum ItemModiferData{
	IDATA_ADD,
	IDATA_HIT,
	IDATA_DAMAGE,
	IDATA_CONSUME,
	ITEM_MOD_DATA_COUNT,
};
enum ItemStrData{
	ISTRDATA_NAME,
	ISTRDATA_DESCRIPTION,
	ITEM_STR_DATA_COUNT,
};
char *bstat_str(enum BStatsData value);
char *dstat_str(enum DStatsData value);
char *edata_str(enum EntityData value);
char *tdata_str(enum InteractableData value);
char *idata_str(enum ItemModiferData value);
char *istrdata_str(enum ItemStrData value);
