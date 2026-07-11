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


/*Todo
 * Write the getter functions for items.*/

#define MAX_ITEMS 512
static struct Item items[MAX_ITEMS] = {0};
static struct local_indx indx_man[MAX_ITEMS] = {0};

const struct BitFlagDef flag_lookup[] = {
	{"throwable",   (1 << FLAG_THROWABLE)},
	{"consumeable", (1 << FLAG_CONSUMEABLE)},
};

static void parse_item_dataset(struct config_pack p, struct ItemDataSet *ds){
	if(t_check(p.key, "stat")){
		ds->stat = (uint8_t)string_to_dev_enum(p.value);
	} else if(t_check(p.key, "amount")){
		ds->amount = (int8_t)atoi(p.value);
	}
}

static void parse_flags(char *value, uint8_t *flags){
	char buf[128];
	strncpy(buf, value, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = '\0';

	char *tok = strtok(buf, " ,|");
	while(tok){
		size_t n = sizeof(flag_lookup) / sizeof(flag_lookup[0]);
		for(size_t i = 0; i < n; i++){
			if(strcmp(tok, flag_lookup[i].string) == 0){
				*flags |= flag_lookup[i].bit;
				break;
			}
		}
		tok = strtok(NULL, " ,|");
	}
}
void item_parser(struct config_pack p, void *ptr){
	struct Item *item = (struct Item *)ptr;

	if(t_check(p.current_section, "item")){
		if(t_check(p.key, "name")){
			h_cpy(item->name, p.value);
		} else if(t_check(p.key, "description")){
			h_cpy(item->description, p.value);
		} else if(t_check(p.key, "flags")){
			parse_flags(p.value, &item->flags);
		} else if(t_check(p.key, "tile_range")){
			item->tile_range = (uint16_t)atoi(p.value);
		}
		return;
	}

	if(t_check(p.current_section, "add")){
		parse_item_dataset(p, &item->add);
		return;
	}
	if(t_check(p.current_section, "use_hit")){
		parse_item_dataset(p, &item->use_hit);
		return;
	}
	if(t_check(p.current_section, "use_damage")){
		parse_item_dataset(p, &item->use_damage);
		return;
	}
	if(t_check(p.current_section, "use_consume")){
		parse_item_dataset(p, &item->use_consume);
		return;
	}
}

bool i_load_item(int gindx){
	int lindx = t_gindx_to_lindx(indx_man, MAX_ITEMS, gindx);
	bool r = t_loader(gindx, indx_man, item_parser, e_get_items_path(), &items[lindx], lindx);
	bool r2 = t_lset_lindx(indx_man, MAX_ITEMS, gindx, lindx);
	return (r && r2);
}

bool i_free_item(int gindx){
	int lindx = t_gindx_to_lindx(indx_man, MAX_ITEMS, gindx);
	// There needs to be error checking for all this
	// Free strings if there are any here
	free(items[lindx].name);	
	free(items[lindx].description);
	items[lindx].name = NULL;
	items[lindx].description = NULL;
	items[lindx] = (struct Item){0};	
	return t_lfree_lindx(indx_man, MAX_ITEMS, lindx);
}
