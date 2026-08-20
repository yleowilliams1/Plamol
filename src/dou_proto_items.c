#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "dou_proto_items.h"

#include "t_config_tool.h"
#include "t_strings.h"
#include "t_log_handler.h"

#include "c_dou_struct_defs.h"
#include "c_data_enums.h"
#include "c_flag_enums.h"

#include "e_dou_manager.h"


static void item_on_init(void *slot);
static void item_on_free(void *slot);
static void item_on_pload(void *slot);
static void item_on_load(struct config_pack p, void *ptr);

struct DouLoader dou_item(){
	return (struct DouLoader){
		.func.on_load = item_on_load,
		.func.on_init = item_on_init,
		.func.on_free = item_on_free,
		.func.on_pload = item_on_pload,
		.size = sizeof(struct DouItemPrototype),
		.type = EOU_ITEM,
	};
}

static void item_on_init(void *slot){
	LOG(LOG_LOAD, "Initalizing item of address %p", slot);
}
static void item_on_free(void *slot){
	struct DouItemPrototype *item = (struct DouItemPrototype *)slot;	
	for(int i = 0; i < ITEM_STR_DATA_COUNT; i++){
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
	struct DouItemPrototype *item = (struct DouItemPrototype *)ptr;
	if(!item){LOG(LOG_NULL, "Took null pointer into parser, This shouldn't be possible");}
	
	for(int i = 0; i < ITEM_MOD_DATA_COUNT; i++){
		if(t_check(p.current_section, idata_str(i))){
			if(t_check(p.key, "DStat")){
				t_atoi(p.value, &item->modifer[i].dstat);	
				if(item->modifer[i].dstat > DERIVED_STAT_COUNT || item->modifer[i].dstat < 0){
					LOG(LOG_PARSE, "%d is not a valid DSTAT value", item->modifer[i].dstat);
					item->modifer[i].dstat = 0;
				}
			}
			else if(t_check(p.key, "BStat")){
				t_atoi(p.value, &item->modifer[i].bstat);
				if(item->modifer[i].bstat > BASE_STAT_COUNT || item->modifer[i].bstat < 0){
					LOG(LOG_PARSE, "%d is not a valid BSTAT value", item->modifer[i].bstat);
					item->modifer[i].bstat = 0;
				}
			}
			if(t_check(p.key, "Amount")){
				t_atoi(p.value, &item->modifer[i].amount);
			}	
		}
	}
	if(t_check(p.current_section, "Strings")){
		for(int i = 0; i < ITEM_STR_DATA_COUNT; i++){
			if(t_check(p.key, istrdata_str(i))){
				t_cpy(&item->strings[i], p.value);
			}
		}
	}
	if(t_check(p.current_section, "General")){ 
		for(int i = 0; i < ITEM_FLAG_COUNT; i++){
			char *str = item_flag_str(i);
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
