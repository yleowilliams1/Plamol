#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "e_engine_settings.h"
#include "t_config_tool.h"
#include "t_gindex_tool.h"
#include "p_stats.h"
#include "i_items.h"
#include "t_strings.h"
#include "e_error_handler.h"
#include "l_asset_manager.h"

#define MAX_ITEMS 512

// Items are immutable !

static void item_parser(struct config_pack p, void *ptr);

static struct Item items[MAX_ITEMS] = {0};
static struct local_indx indx_man[MAX_ITEMS] = {0};

static const char *itemdata_lokup[IDATA_COUNT] = {
	[S_ADD] = "add",
	[S_HIT] = "hit",
	[S_DAMAGE] = "damage",
	[S_CONSUME] = "consume",
	[S_FLAG] = "flags",
	[S_RANGE] = "range",
};

static const char *itemstrs_lokup[ISTR_COUNT] = {
	[S_NAME] = "name",
	[S_DESCRIPTION] = "description",
};

static const char *itemflag_lokup[FLAG_COUNT] = {
	[FLAG_THROWABLE] = "throwable",
	[FLAG_CONSUMEABLE] = "consumeable",
};

bool t_free_item(int gindx){
	// Don't use the asset manager
	// free here since it can't account
	// for malloced strings
	int lindx = t_gindx_to_lindx(indx_man, MAX_ITEMS, gindx);
	if(!t_indxvalid(MAX_ITEMS, lindx)){
		ERR_LOG(ERR_INDX, "Failed gindx: %d conversion", gindx);
		return false;
	}
	for(int i = 0; i < ISTR_COUNT; i++){
		if(items[lindx].strs[i]){
			free(items[lindx].strs[i]);
			items[lindx].strs[i] = NULL;
		}
	}	
	items[lindx] = (struct Item){0};	
	ERR_LOG(ERR_OK, "freed item %d!", gindx);
	return t_lfree_lindx(indx_man, MAX_ITEMS, lindx);

}
bool t_load_item(int gindx){
	
	struct AssetLoadPackage pckg = {
		.gindx = gindx,
		.index_manager = indx_man,
		.arr_cap = MAX_ITEMS,
		.arr = items,
		.element_size = sizeof(struct Item),
		.function = item_parser,
		.path = e_grab_str(ITEMS_PATH),	
		.init = NULL,
	};	
	
	bool success = l_load_asset(pckg);	
	
	ERR_LOG(ERR_OK, "Loaded Item %d!", gindx);
	return success;
}

uint32_t i_get_pckitemdata(int gindx, enum ItemData d, bool autoload){
	int lindx = l_getter_checks(gindx, autoload, MAX_ITEMS, indx_man, t_load_item);
	if(!t_indxvalid(MAX_ITEMS, lindx)){ERR_LOG(ERR_FUCKED, "Couldn't find or load gindx %d", gindx);}
	if(d >= IDATA_COUNT){
		ERR_LOG(ERR_FUCKED, "Sent bad itemdata enum value");
	}
	return items[lindx].dataset[d]; 
}
char *i_get_pckitemstrs(int gindx, enum ItemStrings d, bool autoload){
	int lindx = l_getter_checks(gindx, autoload, MAX_ITEMS, indx_man, t_load_item);
	if(!t_indxvalid(MAX_ITEMS, lindx)){ERR_LOG(ERR_FUCKED, "Couldn't find or load gindx %d", gindx);}
	if(d >= ISTR_COUNT){
		ERR_LOG(ERR_FUCKED, "Sent bad itemdata enum value");
	}
	return items[lindx].strs[d]; 
}

uint32_t pack_dataset(uint16_t a, uint16_t b){
	return((uint32_t) a << 16) | b;
}
struct ItemDataSet unpack(uint32_t packed){
	struct ItemDataSet set = {0};
	set.stat = (packed >> 16) & 0xFFFF;
	set.amount = packed & 0xFFFF;	

	return set;	
}

static void item_parser(struct config_pack p, void *ptr){
	struct Item *item = (struct Item *)ptr;
	if(!item){ERR_LOG(ERR_FUCKED, "Took null poitner into parser, This shouldn't be possible");}
	
	for(int i = 0; i < IDATA_COUNT; i++){
		if(t_check(p.current_section, (char*)itemdata_lokup[i])){
			if(i == S_FLAG){continue;}
			else if(i == S_RANGE){
				if(t_check(p.key, "range_per_tiles")){
					t_atoi(p.value, (int *)&item->dataset[i]);
				}
				continue;
			}
			else if(t_check(p.key, "Stat")){
				int stat; 
				t_atoi(p.value, &stat);
				// Check stat is inbounds
				if(stat >= DERV_CAP || stat < 0){
					continue;
				}
				item->dataset[i] = (item->dataset[i] & 0x0000FFFF) | ((uint32_t)stat << 16);
			}
			else if(t_check(p.key, "Amount")){
				int amount;
				t_atoi(p.value, &amount);
				item->dataset[i] = (item->dataset[i] & 0xFFFF0000) | (amount & 0xFFFF);
			}	
		}
	}
	if(t_check(p.current_section, "Strings")){
		for(int i = 0; i < ISTR_COUNT; i++){
			if(t_check(p.key, (char*)itemstrs_lokup[i])){
				t_cpy(&item->strs[i], p.value);
			}
		}
	}
	if(t_check(p.current_section, (char*)itemdata_lokup[S_FLAG])){
		for(int i = 0; i < FLAG_COUNT; i++){
			char *str = (char *)itemflag_lokup[i];
			if(t_check(p.key, str)){
				int value;
				t_atoi(p.value, &value);
				if(value < 0){ERR_LOG(ERR_PARSE, "Tried to parse flag %s with value of less than 0", str);}
				if(value > 0){item->dataset[S_FLAG] |= (1 << i);}
			}
		}	
	}
}
