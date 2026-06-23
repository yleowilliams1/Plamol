#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "e_engine_settings.h"
#include "t_config_tool.h"
#include "p_stats.h"
#include "i_items.h"

#define MAX_ITEMS 512

struct BitFlagDef{
	char *string;
	uint8_t bit;
};
const struct BitFlagDef flag_lookup[] = {
	{"throwable",   (1 << FLAG_THROWABLE)},
	{"consumeable", (1 << FLAG_CONSUMEABLE)},
};

struct Item items[MAX_ITEMS] = {0};

const struct Item* i_grab_item(int indx){
	return &items[indx];
}

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
			free(item->name);
			item->name = strdup(p.value);
		} else if(t_check(p.key, "description")){
			free(item->description);
			item->description = strdup(p.value);
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

void i_load_item(int indx){
	char *base = e_get_items_path();
	size_t base_len = strlen(base);
	bool needs_slash = (base_len == 0 || base[base_len - 1] != '/');

	/* "%s/%u.ini\0" — base + slash + up to 10 digits + 4 + null */
	char *file = malloc(base_len + needs_slash + 10 + 4 + 1);
	if(!file){return;}

	strcpy(file, base);
	if(needs_slash){file[base_len] = '/'; base_len++;}
	sprintf(file + base_len, "%u.ini", (unsigned int)indx);

	if(t_config(&items[indx], file, item_parser)){
		items[indx].valid = true;
	}

	free(file);
}
