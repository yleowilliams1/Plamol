#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "e_engine_settings.h"
#include "t_config_tool.h"
#include "p_stats.h"
#include "i_inventory.h"
#include "i_items.h"
#include "t_gindex_tool.h"
#define ARR_SIZE 128
/* All of this is terrible
 * I have to localize everything. This is all shit.
 * Fuck i hate everything.
 * This is the offical way of things. This. One load, one free, 
 * load does active bool check.
 * NO FUCKING POINTERS
 * */
static struct Inventory iarr [ARR_SIZE] = {0};
static struct local_indx indx_man[ARR_SIZE] = {0};

static void inventory_parser(struct config_pack p, void *ptr){
	struct Inventory *inv = (struct Inventory *)ptr;
	if(t_check(p.current_section, "hotbar")){
		for(int i = 0; i < HOTBAR_SIZE; i++){
			char buf[32];
			snprintf(buf, sizeof(buf), "%d", i);
			if(t_check(p.key, buf)){
				inv->hotbar_items[i] = atoi(p.value);
			}
		}	
	}
	if(t_check(p.current_section, "inventory")){
		for(int i = 0; i < INVENTORY_SIZE; i++){
			char buf[32];
			snprintf(buf, sizeof(buf), "%d", i);
			if(t_check(p.key, buf)){
				inv->inventory[i] = atoi(p.value);
			}
		}	
	}
}


bool i_free_inventory(int global_indx){
	return t_gfree_lindx(indx_man, ARR_SIZE, global_indx);
}
bool i_load_inventory(int global_index){
	int lindx = t_gindx_to_lindx(indx_man, ARR_SIZE, global_index);
	bool r = t_loader(global_index, indx_man, inventory_parser, e_get_inventory_path(), &iarr[lindx], lindx);
	return r;
}
int i_grab_stat_bonus(enum Stats stat, int indx){
}
int i_grab_dev_bonus(enum Dev dev, int indx){
}




