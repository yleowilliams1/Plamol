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

static const char *dev_lookup[] = {
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
int p_get_dev(int gindx, enum Dev d, bool autoload){
	// The caller basically just needs to trust this. If it fails it returns a 
	// default value and taht gets used.
	int *b = NULL;
	if(autoload){
		int lindx = t_gindx_to_lindx(indx_man, STAT_CAP ,gindx);	
		// Lindx should be NULL
		if(lindx != NULL_INDX){
			ERR_LOG(ERR_NULL, "Tried to autoload gindx %d when already loaded", gindx);
			b = bstats[lindx].basestats;
		}
		else{
			// lindx proved its not loaded
			bool loaded = t_load_stat(gindx);
			// Good thing is this most likely won't
			// pass at least of the writing of this
			// comment the program will exit if 
			// any of these loaded things failed.
			if(!loaded){ERR_LOG(ERR_FUCKED, "Failed to load gindx %d", gindx);}	
			// This isn't super efficent but I can't be bothered to clean it up
			// so it's whatever. It's like tiny loop it doesn't matter.
			int lindx = t_gindx_to_lindx(indx_man, STAT_CAP, gindx);
			if(lindx == NULL_INDX){ERR_LOG(ERR_FUCKED, "Most likely gindx %d was freed since loading.", gindx);}
			b = bstats[lindx].basestats;
		}
	}
	else {
		int lindx = t_gindx_to_lindx(indx_man, STAT_CAP, gindx);
		if(lindx == NULL_INDX){
			ERR_LOG(ERR_INDX, "Tried to get derived stat with autoload toggled off, and didn't preload. gindx %d", gindx);
			// Re-run the function with autload set to true this time.
			return p_get_dev(gindx, d, true);
		}
		b = bstats[lindx].basestats;
	}
		
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
