#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "t_config_tool.h"
#include "dou_proto_inventory.h"
#include "t_log_handler.h"
#include "t_strings.h"
#include "c_magic_number.h"
#include "e_dou_manager.h"

static void inv_on_init(void *slot);
static void inv_on_free(void *slot);
static void inv_on_pload(void *slot);
static void inv_on_load(struct config_pack p, void *ptr);

struct DouLoader dou_inventory(){
	return (struct DouLoader){
		.on_load = inv_on_load,
		.on_init = inv_on_init,
		.on_free = inv_on_free,
		.on_pload = inv_on_pload,
		.size = sizeof(struct Inventory),
		.type = DOU_INV,
	};
}

static void inv_on_init(void *slot){
	struct Inventory *inv = (struct Inventory *)slot;
	for(int i = 0; i < HOTBAR_SIZE; i++){ inv->hotbar_items[i] = EMPTY_SLOT; }
	for(int i = 0; i < INVENTORY_SIZE; i++){ inv->inventory[i] = EMPTY_SLOT; }
	LOG(LOG_LOAD, "Initalizing inventory of address %p", slot);
}
static void inv_on_free(void *slot){
	LOG(LOG_FREE, "Freeing inventory of address %p", slot);
}
static void inv_on_pload(void *slot){
	LOG(LOG_LOAD, "Post-loading inventory of addres %p", slot);
}
static void inv_on_load(struct config_pack p, void *ptr){
	struct Inventory *inv = (struct Inventory *)ptr;
	if(!inv){LOG(LOG_ABORT, "pointer is null.");} 
	if(t_check(p.current_section, "hotbar")){
		for(int i = 0; i < HOTBAR_SIZE; i++){
			char buf[32];
			size_t len;
			bool result = t_snprintf(buf, sizeof(buf), &len, "%d", i);
			if(!result){
				LOG(LOG_NULL, "Failed snprintf for indx %d parsing hotbar", i);
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
				LOG(LOG_NULL, "Failed snprintf for indx %d parsing inventory", i);
				return;
			}
			if(t_check(p.key, buf)){
				t_atoi(p.value, &inv->inventory[i]);
			}
		}	
	}
}




