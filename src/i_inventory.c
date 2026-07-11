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
/*Todo
 * Write the getter functions*/
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
	int lindx = t_gindx_to_lindx(indx_man, ARR_SIZE, global_indx);
	// Free strings if there are any here
	iarr[lindx] = (struct Inventory){0}; 
	return t_lfree_lindx(indx_man, ARR_SIZE, lindx);
}
bool i_load_inventory(int global_index){
	int lindx = t_gindx_to_lindx(indx_man, ARR_SIZE, global_index);
	bool r = t_loader(global_index, indx_man, inventory_parser, e_get_inventory_path(), &iarr[lindx], lindx);
	bool r2 = t_lset_lindx(indx_man, ARR_SIZE, global_index, lindx);
	return (r && r2);
}
int i_grab_stat_bonus(enum Stats stat, int indx){
}
int i_grab_dev_bonus(enum Dev dev, int indx){
}




