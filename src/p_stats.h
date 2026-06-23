#pragma once

enum Stats{
	STR,
	DEX,
	CON,
	SOC,
	INT,
	WIS,
	STAT_SENT
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

struct ParserDef{
	int stat;
	char *name;
};

enum Dev string_to_dev_enum(char *string);
void p_load_stat(int index);
int p_get_dev(int index, enum Dev d);
