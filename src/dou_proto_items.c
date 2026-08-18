#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "t_config_tool.h"
#include "dou_proto_items.h"
#include "t_strings.h"
#include "t_log_handler.h"
#include "e_dou_manager.h"

static const char *itemdata_lokup[IDATA_COUNT] = {
	[S_ADD] = "add",
	[S_HIT] = "hit",
	[S_DAMAGE] = "damage",
	[S_CONSUME] = "consume",
};
static const char *itemstrs_lokup[ISTR_COUNT] = {
	[S_NAME] = "name",
	[S_DESCRIPTION] = "description",
};
static const char *itemflag_lokup[FLAG_COUNT] = {
	[FLAG_THROWABLE] = "throwable",
	[FLAG_CONSUMEABLE] = "consumeable",
};

static void item_on_init(void *slot);
static void item_on_free(void *slot);
static void item_on_pload(void *slot);
static void item_on_load(struct config_pack p, void *ptr);

struct DouLoader dou_item(){
	return (struct DouLoader){
		.on_load = item_on_load,
		.on_init = item_on_init,
		.on_free = item_on_free,
		.on_pload = item_on_pload,
		.size = sizeof(struct Item),
		.type = DOU_ITEM,
	};
}

static void item_on_init(void *slot){
	LOG(LOG_LOAD, "Initalizing item of address %p", slot);
}
static void item_on_free(void *slot){
	struct Item *item = (struct Item *)slot;	
	for(int i = 0; i < ISTR_COUNT; i++){
		if(item->strs[i]){
			free(item->strs[i]);
			item->strs[i] = NULL;
		}
	}	
	LOG(LOG_FREE, "Freeing item of address %p", slot);
}
static void item_on_pload(void *slot){
	LOG(LOG_LOAD, "Post-loading item of address %p", slot);
}
static void item_on_load(struct config_pack p, void *ptr){
	struct Item *item = (struct Item *)ptr;
	if(!item){LOG(LOG_NULL, "Took null pointer into parser, This shouldn't be possible");}
	
	for(int i = 0; i < IDATA_COUNT; i++){
		if(t_check(p.current_section, (char*)itemdata_lokup[i])){
			if(t_check(p.key, "Stat")){
				t_atoi(p.value, &item->dataset[i].stat);
			}
			if(t_check(p.key, "Amount")){
				t_atoi(p.value, &item->dataset[i].amount);
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
	if(t_check(p.current_section, "General")){ 
		for(int i = 0; i < FLAG_COUNT; i++){
			char *str = (char *)itemflag_lokup[i];
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
