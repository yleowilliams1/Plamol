#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "t_config_tool.h"
#include "dou_proto_stats.h"
#include "t_log_handler.h"
#include "t_strings.h"
#include "e_dou_manager.h"
#include "c_types.h"
static const char *base_lookup[] = {
	[STR] = "strength",
	[DEX] = "dexterity",
	[CON] = "constitution",
	[SOC] = "social",
	[INT] = "intelligence",
	[WIS] = "wisdom",
};

static void stat_on_init(void *slot);
static void stat_on_free(void *slot);
static void stat_on_pload(void *slot);
static void stat_on_load(struct config_pack p, void *ptr);

struct DouLoader dou_stat(){
	return (struct DouLoader){
		.on_load = stat_on_load,
		.on_init = stat_on_init,
		.on_free = stat_on_free,
		.on_pload = stat_on_pload,
		.size = sizeof(struct BaseStats),
		.type = DOU_STAT,
	};
}
	
static void stat_on_init(void *slot){
	LOG(LOG_LOAD, "Initalizing stat of address %p", slot);
}
static void stat_on_free(void *slot){
	LOG(LOG_FREE, "Freeing stat of address %p", slot);
}
static void stat_on_pload(void *slot){
	LOG(LOG_LOAD, "Post-loading stat of address %p", slot);
}
static void stat_on_load(struct config_pack p, void *ptr){
	struct BaseStats *bstat = (struct BaseStats *)ptr;
	if(!bstat){LOG(LOG_NULL, "Passed null pointer to parser"); return;}	
	
	if(t_check(p.current_section, "stats")){
		for(int i = 0; i < BSTAT_COUNT; i++){
			char *str = (char *)base_lookup[i];

			if(t_check(p.key, str)){
				t_atoi(p.value, &bstat->basestats[i]);
			}
		}
	}	
}
