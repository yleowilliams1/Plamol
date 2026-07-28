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
	DERV_CAP
};
struct BaseStats{
	int basestats[BSTAT_COUNT];
};

bool t_load_stat(int gindx);
bool t_free_stat(int gindx);
bool p_get_dev(int gindx, enum Dev d, bool autoload, int *out);
