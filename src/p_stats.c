#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "t_config_tool.h"
#include "p_stats.h"
#include "e_engine_settings.h"
#define SIZE 256

static int base_stats[SIZE][STAT_SENT] = {0};

const struct ParserDef base_lookup[] = {
	{STR, "strength"},
	{DEX, "dexterity"},
	{CON, "constitution"},
	{SOC, "social"},
	{INT, "intelligence"},
	{WIS, "wisdom"},
};

const struct ParserDef dev_lookup[] = {
        {PHYSICAL_COORDINATION,  "physical_coordination"},
        {WORD,                   "word"},
        {PROB_ANALYSIS,          "prob_analysis"},
        {SPATIAL,                "spatial"},
        {MUSICAL,                "musical"},
        {NATURAL,                "natural"},
        {INTERPERSONAL,          "interpersonal"},
        {INTRAPERSONAL,          "intrapersonal"},
        {INNOCENCE,              "innocence"},
        {HEROISM,                "heroism"},
        {LOVE,                   "love"},
        {AUTHORITARIAN,          "authoritarian"},
        {AC,                     "ac"},
        {DR,                     "dr"},
        {MAX_HP,                 "max_hp"},
        {INITIATIVE,             "initiative"},
};
enum Dev string_to_dev_enum(char *string){
	for(int i = 0; i < DEV_SENT; i++){
		if(strcmp(dev_lookup[i].name, string) == 0){
			return dev_lookup[i].stat;				
		}
	}

	return -1;
}

void stat_parser(struct config_pack p, void *ptr){
	char *v = p.value;
	int *stat = (int *)ptr;
	if(t_check(p.current_section, "stats")){
		for(int i = 0; i < sizeof(base_lookup) / sizeof(base_lookup[0]); i++){
			const struct ParserDef *d = &base_lookup[i];
			if(!t_check(p.key, d->name)){continue;}
			stat[d->stat] = atoi(v);	
		}
	}	
}

void p_load_stats(int indx){
	char *base = e_get_stats_path();
	size_t base_len = strlen(base);
	bool needs_slash = (base_len == 0 || base[base_len - 1] != '/');
	char *file = malloc(base_len + needs_slash + 10 + 4 + 1);
	if(!file){return;}
	strcpy(file, base);
	if(needs_slash){file[base_len] = '/'; base_len++;}
	sprintf(file + base_len, "%u.ini", (unsigned int)indx);
	t_config(&base_stats[indx], file, stat_parser);
	free(file);
}

int p_get_dev(int index, enum Dev d){
	int *b = base_stats[index];
	switch(d){
			case PHYSICAL_COORDINATION:
					return (int)ceil(((b[STR] + b[DEX] - b[INT]) / 2.0) / 4.0);
			case WORD:
					return (int)ceil(((b[SOC] + b[INT] - b[CON]) / 2.0) / 4.0);
			case PROB_ANALYSIS:
					return (int)ceil(((b[INT] + b[WIS] - b[STR]) / 2.0) / 4.0);
			case SPATIAL:
					return (int)ceil(((b[INT] + b[DEX] - b[SOC]) / 2.0) / 4.0);
			case MUSICAL:
					return (int)ceil(((b[WIS] + b[DEX] - b[SOC]) / 2.0) / 4.0);
			case NATURAL:
					return (int)ceil(((b[WIS] + b[CON] - b[DEX]) / 2.0) / 4.0);
			case INTERPERSONAL:
					return (int)ceil(((b[SOC] + b[WIS] - b[STR]) / 2.0) / 4.0);
			case INTRAPERSONAL:
					return (int)ceil(((b[INT] + b[CON] - b[WIS]) / 2.0) / 4.0);
			case INNOCENCE:
					return (int)ceil(((b[CON] + b[DEX] - b[INT]) / 2.0) / 4.0);
			case HEROISM:
					return (int)ceil(((b[STR] + b[SOC] - b[INT]) / 2.0) / 4.0);
			case LOVE:
					return (int)ceil(((b[SOC] + b[CON] - b[DEX]) / 2.0) / 4.0);
			case AUTHORITARIAN:
					return (int)ceil(((b[STR] + b[INT] - b[SOC]) / 2.0) / 4.0);
			case AC:
					return (int)ceil((b[DEX] + b[WIS]) - b[STR]);
			case DR:
					return (int)ceil((b[STR] + b[CON]) - b[DEX]);
			case MAX_HP:
					return (int)ceil((b[CON] + b[STR]) - b[DEX]);
			case INITIATIVE:
					return (int)ceil(((b[DEX] + b[WIS]) - b[INT]) / 4.0);
			default:
					return 0;
	}
}
