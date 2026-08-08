#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "t_config_tool.h"
#include "p_stats.h"
#include "e_engine_settings.h"
#include "t_gindex_tool.h"
#include "e_error_handler.h"
#include "t_strings.h"
#include "l_asset_manager.h"
#define STAT_CAP 256

// Stats are mutable

static void stat_parser(struct config_pack p, void *ptr);

static struct BaseStats bstats[STAT_CAP] = {0};
static struct local_indx indx_man[STAT_CAP] = {0};

static const char *base_lookup[] = {
	[STR] = "strength",
	[DEX] = "dexterity",
	[CON] = "constitution",
	[SOC] = "social",
	[INT] = "intelligence",
	[WIS] = "wisdom",
};

struct BaseStats s_grab_stats(int gindx, bool autoload){
	int lindx = l_getter_checks(gindx, autoload, STAT_CAP, indx_man, t_load_stat);
	if(!t_indxvalid(STAT_CAP, lindx)){ERR_LOG(ERR_FUCKED, "Couldn't load or find gindx %d", gindx);}
	return bstats[lindx];
}

static const char *dev_lookup[DERV_CAP] = {
        [PHYSICAL_COORDINATION]  = "physical_coordination",
        [WORD]                   = "word",
        [PROB_ANALYSIS]          = "prob_analysis",
        [SPATIAL]                = "spatial",
        [MUSICAL]                = "musical",
        [NATURAL]                = "natural",
        [INTERPERSONAL]          = "interpersonal",
        [INTRAPERSONAL]          = "intrapersonal",
        [INNOCENCE]              = "innocence",
        [HEROISM]                = "heroism",
        [LOVE]                   = "love",
        [AUTHORITARIAN]          = "authoritarian",
        [AC]                     = "ac",
        [DR]                     = "dr",
        [MAX_HP]	         = "max_hp",
        [INITIATIVE]             = "initiative",
};

bool t_load_stat(int gindx){
	struct AssetLoadPackage pckg = {
		.gindx = gindx,
		.index_manager = indx_man,
		.arr_cap = STAT_CAP,
		.arr = bstats,
		.element_size = sizeof(struct BaseStats),
		.function = stat_parser,
		.path = e_grab_str(STATS_PATH),	
		.init = NULL,
	};

	return l_load_asset(pckg);
}
bool t_free_stat(int gindx){
	
	struct AssetFreePackage pckg = {
		.gindx = gindx,
		.index_manager = indx_man,
		.arr_cap = STAT_CAP,
		.arr = bstats,
		.element_size = sizeof(struct BaseStats),
	};

	return t_free_asset(pckg);
}
static void stat_parser(struct config_pack p, void *ptr){
	struct BaseStats *bstat = (struct BaseStats *)ptr;
	if(!bstat){ERR_LOG(ERR_FUCKED, "Passed null pointer to parser");}	
	
	if(t_check(p.current_section, "stats")){
		for(int i = 0; i < BSTAT_COUNT; i++){
			char *str = (char *)base_lookup[i];

			if(t_check(p.key, str)){
				t_atoi(p.value, &bstat->basestats[i]);
			}
		}
	}	
}
bool p_get_dev(int *b, enum Dev d, int *out){
	switch(d){
		case PHYSICAL_COORDINATION:
			*out = (int)ceil(((b[STR] + b[DEX] - b[INT]) / 2.0) / 4.0);
			break;
		case WORD:
			*out = (int)ceil(((b[SOC] + b[INT] - b[CON]) / 2.0) / 4.0);
			break;
		case PROB_ANALYSIS:
			*out = (int)ceil(((b[INT] + b[WIS] - b[STR]) / 2.0) / 4.0);
			break;
		case SPATIAL:
			*out = (int)ceil(((b[INT] + b[DEX] - b[SOC]) / 2.0) / 4.0);
			break;
		case MUSICAL:
			*out = (int)ceil(((b[WIS] + b[DEX] - b[SOC]) / 2.0) / 4.0);
			break;
		case NATURAL:
			*out = (int)ceil(((b[WIS] + b[CON] - b[DEX]) / 2.0) / 4.0);
			break;
		case INTERPERSONAL:
			*out = (int)ceil(((b[SOC] + b[WIS] - b[STR]) / 2.0) / 4.0);
			break;
		case INTRAPERSONAL:
			*out = (int)ceil(((b[INT] + b[CON] - b[WIS]) / 2.0) / 4.0);
			break;
		case INNOCENCE:
			*out = (int)ceil(((b[CON] + b[DEX] - b[INT]) / 2.0) / 4.0);
			break;
		case HEROISM:
			*out = (int)ceil(((b[STR] + b[SOC] - b[INT]) / 2.0) / 4.0);
			break;
		case LOVE:
			*out = (int)ceil(((b[SOC] + b[CON] - b[DEX]) / 2.0) / 4.0);
			break;
		case AUTHORITARIAN:
			*out = (int)ceil(((b[STR] + b[INT] - b[SOC]) / 2.0) / 4.0);
			break;
		case AC:
			*out = (int)ceil((b[DEX] + b[WIS]) - b[STR]);
			break;
		case DR:
			*out = (int)ceil((b[STR] + b[CON]) - b[DEX]);
			break;
		case MAX_HP:
			*out = (int)ceil((b[CON] + b[STR]) - b[DEX]);
			break;
		case INITIATIVE:
			*out = (int)ceil(((b[DEX] + b[WIS]) - b[INT]) / 4.0);
			break;
		default:
			*out = 0;
			break;
	}
	return true;
}
