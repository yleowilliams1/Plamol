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
#include "e_error_handler.h"
#include "t_strings.h"
#define ARR_SIZE 128
/*Todo
 * Write the getter functions
 * Have safe attoi with the print functions inm string.h*/
static struct Inventory iarr [ARR_SIZE] = {0};
static struct local_indx indx_man[ARR_SIZE] = {0};

static void inventory_parser(struct config_pack p, void *ptr){
	struct Inventory *inv = (struct Inventory *)ptr;
	if(!inv){ERR_LOG(ERR_FUCKED, "pointer is null. Fucked up...");}
	if(t_check(p.current_section, "hotbar")){
		for(int i = 0; i < HOTBAR_SIZE; i++){
			char buf[32];
			size_t len;
			bool result = t_snprintf(buf, sizeof(buf), &len, "%d", i);
			if(!result){
				ERR_LOG(ERR_NULL, "Failed snprintf for indx %d parsing hotbar, Please check log above for snprintf log errors. If there aren't any your fucked sorry dude.", i);
				return;
			}
			if(t_check(p.key, buf)){
				t_atoi(p.value, &inv->hotbar_items[i]);
			}
		}	
	}
	if(t_check(p.current_section, "inventory")){
		for(int i = 0; i < INVENTORY_SIZE; i++){
			char buf[32];
			size_t len;
			bool result = t_snprintf(buf, sizeof(buf), &len, "%d", i);
			if(!result){
				ERR_LOG(ERR_NULL, "Failed snprintf for indx %d parsing inventory. Plese check log above for snprintf log errors. If there aren't any your fucked sorry dude.", i);
				return;
			}
			if(t_check(p.key, buf)){
				t_atoi(p.value, &inv->inventory[i]);
			}
		}	
	}
}


bool i_free_inventory(int global_indx){
	int lindx = t_gindx_to_lindx(indx_man, ARR_SIZE, global_indx);
	if(lindx == NULL_INDX){
		ERR_LOG(ERR_NULL, "Failed global to local index conversion for global indx %d", global_indx);
		return false;
	}
	// Free strings if there are any here
	iarr[lindx] = (struct Inventory){0}; 
	return t_lfree_lindx(indx_man, ARR_SIZE, lindx);
}
bool i_load_inventory(int global_index){
	int lindx = t_gindx_to_lindx(indx_man, ARR_SIZE, global_index);
	if(lindx == NULL_INDX){
		// Most likely lindx is NULL due to outofbounds,
		// of course it's not unlikely it's something else.
		// If it's NULL it will have already produced an error message.	
		ERR_LOG(ERR_NULL, "Failed global to local index conversion for global indx %d", global_index);
		return false;
	}
	iarr[lindx] = (struct Inventory){0};
	bool r = t_loader(global_index, indx_man, inventory_parser, e_grab_str(INVENTORY_PATH), &iarr[lindx], lindx);
	bool r2 = t_lset_lindx(indx_man, ARR_SIZE, global_index, lindx);
	if(r == false|| r2 == false){
		ERR_LOG(ERR_PARSE, "Failed to free inventory global index &", global_index);	
	}
	return (r && r2);
}
int i_grab_stat_bonus(enum Stats stat, int indx){
}
int i_grab_dev_bonus(enum Dev dev, int indx){
}




