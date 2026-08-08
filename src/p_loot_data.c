#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "e_engine_settings.h"
#include "t_gindex_tool.h"
#include "i_items.h"
#include "e_error_handler.h"
#include "l_asset_manager.h"
#include "p_loot_data.h"
#include "t_strings.h"
#define LOOT_CAP 128

static void loot_parser(struct config_pack p, void *ptr);

static struct LootData loot[LOOT_CAP] = {0};
static struct local_indx iman[LOOT_CAP] = {0};

bool p_free_loot(int gindx){
	struct AssetFreePackage pckg = {
		.gindx = gindx,
		.index_manager = iman,
		.arr_cap = LOOT_CAP,
		.arr = loot,
		.element_size = sizeof(struct LootData),
	};

	return t_free_asset(pckg);
}

bool p_load_loot(int gindx){
	struct AssetLoadPackage pckg = {
		.gindx = gindx,
		.index_manager = iman,
		.arr_cap = LOOT_CAP,
		.arr = loot,
		.element_size = sizeof(struct LootData),
		.function = loot_parser,
		.path = e_grab_str(LOOT_PATH),
		.init = NULL,
	};

	return l_load_asset(pckg);
}
static void loot_parser(struct config_pack p, void *ptr){
	struct LootData *l = (struct LootData *)ptr;
	if(t_check(p.current_section, "general")){
		if(t_check(p.key, "is_random")){
			int val;
			t_atoi(p.value, &val);
			if(val > 0){l->is_random = true;}
		}
	}
	for(int i = 0; i < LOOT_CAP; i++){
		char buf[32];
		size_t len;
		bool result = t_snprintf(buf, sizeof(buf), &len, "%d", i);
		if(!result){continue;}
		if(!t_check(p.key, buf)){continue;}
		t_atoi(p.value, &l->items_gindx[i]);
	}
}
struct LootData p_grab_loot(int gindx, bool autoload){
	struct LootData data = {0};
	int lindx = l_getter_checks(gindx, autoload, LOOT_CAP, iman, p_load_loot);
	if(!t_indxvalid(LOOT_CAP, lindx)){ERR_LOG(ERR_NULL, "Couldn't grab loot of gindx %d", gindx); return data;}
	data = loot[lindx];
	return data;
}
