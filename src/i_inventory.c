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
#include "l_asset_manager.h"

#define INV_CAP 128
#define EMPTY_SLOT -1

static void inv_parser(struct config_pack p, void *ptr);

static struct Inventory iarr [INV_CAP] = {0};
static struct local_indx indx_man[INV_CAP] = {0};
static void inv_init(void *ptr){
	struct Inventory *inv = (struct Inventory *)ptr;
	for(int i = 0; i < HOTBAR_SIZE; i++){ inv->hotbar_items[i] = EMPTY_SLOT; }
	for(int i = 0; i < INVENTORY_SIZE; i++){ inv->inventory[i] = EMPTY_SLOT; }
}
bool i_free_inventory(int gindx){

	struct AssetFreePackage pckg = {
		.gindx = gindx,
		.index_manager = indx_man,
		.arr_cap = INV_CAP,
		.arr = iarr,
		.element_size = sizeof(struct Inventory),
	};

	ERR_LOG(ERR_OK, "Freed inventory");
	return t_free_asset(pckg);
}
bool i_load_inventory(int gindx){
	struct AssetLoadPackage pckg = {
		.gindx = gindx,
		.index_manager = indx_man,
		.arr_cap = INV_CAP,
		.arr = iarr,
		.element_size = sizeof(struct Inventory),
		.function = inv_parser,
		.path = e_grab_str(INVENTORY_PATH),
		.init = inv_init,
	};
	
	ERR_LOG(ERR_OK, "Loaded inventory");
	return l_load_asset(pckg);
}

struct Inventory i_get_inv_proto(int gindx, bool autoload){
	struct Inventory inv = {0};
	int lindx = l_getter_checks(gindx, autoload, INV_CAP, indx_man, i_load_inventory);
	if(!t_indxvalid(INV_CAP, lindx)){
		ERR_LOG(ERR_NULL, "Couldn't find or load %d", gindx); 
		return inv;
	}

	inv = iarr[lindx];
	return inv;
}

struct DervBonusMatrix i_get_bonus_matrx(int gindx, bool autoload){
	struct DervBonusMatrix matrx = {0};
	struct Inventory inv = i_get_inv_proto(gindx, autoload);
	
	for(int i = 0; i < HOTBAR_SIZE; i++){
		if(inv.hotbar_items[i] == EMPTY_SLOT){
			continue;
		}
		
		// Kind of complicated: Loop through through
		// the hotbar_items and grab the the additive value for 
		// items in hotbar in packed format. 
		// Then unpack the data into the stat and amount;
		uint32_t packed = i_get_pckitemdata(inv.hotbar_items[i], S_ADD, true);
		struct ItemDataSet data = unpack(packed);	
		// Now we have the data and the amount 
		// and stat we can add to the matrix
		// also the stat can't be out of boudns it gets 
		// checked at the item parser
		matrx.derv[data.stat] += data.amount;
	}
	
	// Note this both loades the inventory and
	// the item into memory. So 
	// the caller needs to free it if possible.
	return matrx;
}

static void inv_parser(struct config_pack p, void *ptr){
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




