#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "c_magic_number.h"
#include "t_config_tool.h"
#include "t_log_handler.h"
#include "dou_proto_loot.h"
#include "t_strings.h"
#include "e_dou_manager.h"


static void loot_on_init(void *slot);
static void loot_on_free(void *slot);
static void loot_on_pload(void *slot);
static void loot_on_load(struct config_pack p, void *ptr);
	
struct DouLoader dou_loot(){
	return (struct DouLoader){
		.on_load = loot_on_load,
		.on_init = loot_on_init,
		.on_free = loot_on_free,
		.on_pload = loot_on_pload,
		.size = sizeof(struct LootData),
		.type = DOU_LOOT,
	};
}

static void loot_on_init(void *slot){
	LOG(LOG_LOAD, "Initalizing loot of address %p", slot);
}
static void loot_on_free(void *slot){
	LOG(LOG_FREE, "Freeing loot of address %p", slot);
}
static void loot_on_pload(void *slot){
	LOG(LOG_LOAD, "Post-Loading loot of address %p", slot);
}
static void loot_on_load(struct config_pack p, void *ptr){
	struct LootData *l = (struct LootData *)ptr;
	if(t_check(p.current_section, "general")){
		if(t_check(p.key, "is_random")){
			int val;
			t_atoi(p.value, &val);
			if(val > 0){l->is_random = true;}
		}
	}
	for(int i = 0; i < LOOT_MAX_SIZE; i++){
		char buf[32];
		size_t len;
		bool result = t_snprintf(buf, sizeof(buf), &len, "%d", i);
		if(!result){continue;}
		if(!t_check(p.key, buf)){continue;}
		t_atoi(p.value, &l->items_gindx[i]);
	}
}
