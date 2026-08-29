#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "depo_proto_items.h"

#include "c_stat_list.h"

#include "t_config_tool.h"
#include "t_strings.h"
#include "t_log_handler.h"
#include "t_depot_manager.h"


static void item_on_init(void *slot);
static void item_on_free(void *slot);
static void item_on_pload(void *slot);
static void item_on_load(struct config_pack p, void *ptr);

struct ItemFunctions prototype_item(){
	return (struct ItemFunctions){
		.on_load = item_on_load,
		.on_init = item_on_init,
		.on_free = item_on_free,
		.on_pload = item_on_pload,
	};
}
static void item_on_init(void *slot){
	LOG(LOG_LOAD, "Initalizing item of address %p", slot);
}
static void item_on_free(void *slot){
	struct ItemPrototype *item = (struct ItemPrototype *)slot;	
	for(int i = 0; i < ITEM_STRING_COUNT; i++){
		if(item->strings[i]){
			free(item->strings[i]);
			item->strings[i] = NULL;
		}
	}	
	LOG(LOG_FREE, "Freeing item of address %p", slot);
}
static void item_on_pload(void *slot){
	LOG(LOG_LOAD, "Post-loading item of address %p", slot);
}
static void item_on_load(struct config_pack p, void *ptr){
	struct ItemPrototype *item = (struct ItemPrototype*)ptr;
	if(!item){LOG(LOG_NULL, "Took null pointer into parser, This shouldn't be possible");}
	
	for(int i = 0; i < MODIFIER_COUNT; i++){
		if(t_check(p.current_section, (char *)modstr(i))){
			if(t_check(p.key, "DStat")){
				t_atoi(p.value, &item->mods[i].stat);	
				if(item->mods[i].stat > DERIVED_STAT_COUNT || item->mods[i].stat < 0){
					LOG(LOG_PARSE, "%d is not a valid DSTAT value", item->mods[i].stat);
					item->mods[i].stat = 0;
				}
				item->mods[i].type = BaseStatType;
			}
			else if(t_check(p.key, "BStat")){
				t_atoi(p.value, &item->mods[i].stat);
				if(item->mods[i].stat > BASE_STAT_COUNT || item->mods[i].stat < 0){
					LOG(LOG_PARSE, "%d is not a valid BSTAT value", item->mods[i].stat);
					item->mods[i].stat = 0;
				}
				item->mods[i].type = DerivedStatType;
			}
			if(t_check(p.key, "Amount")){
				t_atoi(p.value, &item->mods[i].amount);
			}	
		}
	}
	if(t_check(p.current_section, "Strings")){
		for(int i = 0; i < ITEM_STRING_COUNT; i++){
			if(t_check(p.key, (char *)itemstrstr(i))){
				t_cpy(&item->strings[i], p.value);
			}
		}
	}
	if(t_check(p.current_section, "General")){ 
		for(int i = 0; i < ITEM_FLAG_COUNT; i++){
			char *str = (char *)itemflgstr(i);
			if(t_check(p.key, str)){
				int value;
				t_atoi(p.value, &value);
				if(value > 0){item->flags |= (1 << i);}
			}
		}
		if(t_check(p.key, "Range")){
			t_atoi(p.value, &item->range);
		}	
	}
}
