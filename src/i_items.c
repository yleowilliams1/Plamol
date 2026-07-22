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
static struct Item items[MAX_ITEMS] = {0};
static struct local_indx indx_man[MAX_ITEMS] = {0};

const struct BitFlagDef flag_lookup[] = {
	{"throwable",   (1 << FLAG_THROWABLE)},
	{"consumeable", (1 << FLAG_CONSUMEABLE)},
};

static void parse_item_dataset(struct config_pack p, struct ItemDataSet *ds){
	if(t_check(p.key, "stat")){
		int stat = string_to_dev_enum(p.value);	
		
		if(stat == NULL_INDX || stat < 0){
			ERR_LOG(ERR_PARSE, "Key %s returned a null enum value", p.value);
		}
		ds->stat = stat;
	} else if(t_check(p.key, "amount")){
		ds->amount = NULL_ATOI;
		t_atoi(p.value, &ds->amount);
		// This atoi will freak the fuck out at the slightest 
		// inconvience so no error checking
	}
}

static void parse_flags(char *value, uint8_t *flags){
	char buf[128];
	size_t buflen;
	t_snprintf(buf, sizeof(buf), &buflen, "%s", value);
	
	if(!value){ERR_LOG(ERR_PARSE, "Passed null value ptr"); return;}	
	if(!flags){ERR_LOG(ERR_PARSE, "Passed null flags ptr"); return;}
	// This will fuck you if you call this for two different strings.
	// If I ever need strtok for super intensive multi-string
	// process I need to write a t_strtok in t_strings.h
	// But for now this is fine.
	char *tok = strtok(buf, " ,|");
	while(tok){
		// Someone could manually delete the strings in the lookup table
		// and then the strcmp probably fucks everything up but 
		// hopefully that doesn't happen so i'm not error checking	
		for(int i = 0; i < FLAG_COUNT; i++){
			if(t_check(tok, flag_lookup[i].string)){
				*flags |= flag_lookup[i].bit;
				break;
			}
		}
		tok = strtok(NULL, " ,|");
	}
}
static void item_parser(struct config_pack p, void *ptr){
	// Rule of thumb is no error checking in parsers since they get carried 
	// in the called functions. Unless your calling straight standard
	// library functions then just let the tools call error log for you.
	// But don't forget to check error when acceessing
	struct Item *item = (struct Item *)ptr;
	if(!item){
		ERR_LOG(ERR_FUCKED, "Took a null pointer to item parser. This shouldn't be possible!");
	}
	if(t_check(p.current_section, "item")){
		if(t_check(p.key, "name")){
			h_cpy(&item->name, p.value);
		} else if(t_check(p.key, "description")){
			h_cpy(&item->description, p.value);
		} else if(t_check(p.key, "flags")){
			parse_flags(p.value, &item->flags);
		} else if(t_check(p.key, "tile_range")){
			item->tile_range = NULL_ATOI;
			t_atoi(p.value, &item->tile_range);
		}	
	}

	if(t_check(p.current_section, "add")){
		parse_item_dataset(p, &item->add);	
	}
	if(t_check(p.current_section, "use_hit")){
		parse_item_dataset(p, &item->use_hit);	
	}
	if(t_check(p.current_section, "use_damage")){
		parse_item_dataset(p, &item->use_damage);
	}
	if(t_check(p.current_section, "use_consume")){
		parse_item_dataset(p, &item->use_consume);
	}
}

bool i_load_item(int gindx){
	
	struct AssetLoadPackage pckg = {
		.gindx = gindx,
		.index_manager = indx_man,
		.arr_cap = MAX_ITEMS,
		.arr = items,
		.element_size = sizeof(struct Item),
		.function = item_parser,
		.path = e_grab_str(ITEMS_PATH),	
	};	
	
	bool success = l_load_asset(pckg);	
	
	return success;
}

bool i_free_item(int gindx){
	// Don't use the asset manager
	// free here since it can't account
	// for malloced strings
	int lindx = t_gindx_to_lindx(indx_man, MAX_ITEMS, gindx);
	if(lindx == NULL_INDX || lindx < 0){
		ERR_LOG(ERR_INDX, "Failed gindx: %d conversion", gindx);
		return false;
	}
	if(items[lindx].name){
		free(items[lindx].name);	
		items[lindx].name = NULL;
	}
	if(items[lindx].description){
		free(items[lindx].description);
		items[lindx].description = NULL;
	}
	items[lindx] = (struct Item){0};	
	return t_lfree_lindx(indx_man, MAX_ITEMS, lindx);
}
