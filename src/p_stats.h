#pragma once
#include <stdbool.h>

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
	PHYSICAL_COORDINATION,
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
	MAX_HP,
	INITIATIVE,
	DEV_SENT
};
struct BaseStats{
	int basestats[BSTAT_COUNT];
};

enum Dev string_to_dev_enum(const char *str);
