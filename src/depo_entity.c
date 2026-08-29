#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "c_stat_list.h"

#include "depo_entity.h"

#include "t_log_handler.h"
#include "t_strings.h"
#include "t_depot_manager.h"
#include "t_config_tool.h"
#include "t_instance_manager.h"
#define INVALID_CAP -1

static void prototype_on_free(void *slot);
static void prototype_on_init(void *slot);
static void prototype_on_ploader(void *slot);
static void prototype_on_loader(struct config_pack p, void *ptr);

static void instance_on_serialize(void *slot, FILE *file);
static void instance_on_deserialize(void *slot, FILE *file);
static void instance_on_free(void *slot);

struct ItemFunctions entity_prototype(){
	return (struct ItemFunctions){
		.on_load = prototype_on_loader,
		.on_init = prototype_on_init,
		.on_free = prototype_on_free,
		.on_pload = prototype_on_ploader,
	};
}
struct InstanceFunctions entity_instance(){
	return (struct InstanceFunctions){
		.on_serialize = instance_on_serialize,
		.on_deserialize = instance_on_deserialize,
		.on_free = instance_on_free,
	};
}

static void instance_on_serialize(void *slot, FILE *f){
	struct EntityPrototype *e = slot;
	fwrite(&e->flags, sizeof(uint32_t), 1, f);
	fwrite(&e->hp, sizeof(int), 1, f);
	fwrite(&e->ap, sizeof(int), 1, f);
	fwrite(e->bstat_data, sizeof(int), BASE_STAT_COUNT, f);

	fwrite(&e->inventory_cap, sizeof(int), 1, f);
	fwrite(e->inventory, sizeof(struct ChildInventorySlot), e->inventory_cap, f);
	fwrite(&e->hotbar_cap, sizeof(int), 1, f);
	fwrite(e->hotbar, sizeof(struct ChildInventorySlot), e->hotbar_cap, f);
}
static void instance_on_deserialize(void *slot, FILE *f){
	struct EntityPrototype *e = slot;
	fread(&e->flags, sizeof(uint32_t), 1, f);
	fread(&e->hp, sizeof(int), 1, f);
	fread(&e->ap, sizeof(int), 1, f);
	fread(e->bstat_data, sizeof(int), BASE_STAT_COUNT, f);

	int inv_size; fread(&inv_size, sizeof(int), 1, f);
	if(inv_size != e->inventory_cap){LOG(LOG_OUTOFBOUNDS, "Inventory size mismatch on load"); return;}
	fread(e->inventory, sizeof(struct ChildInventorySlot), inv_size, f);

	int hot_size; fread(&hot_size, sizeof(int), 1, f);
	if(hot_size != e->hotbar_cap){LOG(LOG_OUTOFBOUNDS, "Hotbar size mismatch on load"); return;}
	fread(e->hotbar, sizeof(struct ChildInventorySlot), hot_size, f);
}
static void instance_on_free(void *slot){
	prototype_on_free(slot);
}

static void prototype_on_free(void *slot){
	struct EntityPrototype *e = (struct EntityPrototype*)slot;
	if(!e){LOG(LOG_NULL, "Took null pointer ."); return;}	
	if(e->inventory){free(e->inventory); e->inventory = NULL;}
	if(e->hotbar){free(e->hotbar); e->hotbar = NULL;}	
}
static void prototype_on_init(void *slot){
	struct EntityPrototype *e = (struct EntityPrototype*)slot;
	if(!e){LOG(LOG_NULL, "Took null pointer ."); return;}	
	e->inventory_cap = INVALID_CAP;
	e->hotbar_cap = INVALID_CAP;
}
static void prototype_on_ploader(void *slot){
	struct EntityPrototype *e = (struct EntityPrototype*)slot;
	if(!e){LOG(LOG_NULL, "Took null pointer ."); return;}	
}
static void prototype_on_loader(struct config_pack p, void *ptr){
	struct EntityPrototype *e = (struct EntityPrototype*)ptr;	
	if(!e){LOG(LOG_NULL, "Took null pointer ."); return;}	
	
	if(t_check(p.current_section, "general")){
		if(t_check(p.key, "SpriteGindx")){t_atoi(p.value, &e->sprite_gindx);}
		if(t_check(p.key, "InventoryCap")){t_atoi(p.value, &e->inventory_cap);}
		if(t_check(p.key, "HotbarCap")){t_atoi(p.value, &e->hotbar_cap);}
	}
	if(t_check(p.current_section, "flags")){
		for(int i = 0; i < ENTITY_FLAG_COUNT; i++){
			char *str = (char *)entflgstr(i);
			if(!t_check(p.key, str)){continue;}
			int value;
			t_atoi(p.value, &value);
			if(value > 0){e->flags |= (1<< i);}
		}
	}
	if(t_check(p.current_section, "inventory_data")){
		if(e->inventory_cap == INVALID_CAP){LOG(LOG_NULL, "Inventory cap is invalid while parsing inventory data, please place general section before the inventory data in the prototype ini"); return;}
		if(!e->inventory){e->inventory = XCALLOC(1, sizeof(struct ChildInventorySlot) * e->inventory_cap);}
		for(int i = 0; i < e->inventory_cap; i++){
			size_t size = 128;
			// Theres a hypthetical max size here, it's something like 999 so when i have time i need to check for that but right now i can't be bothered: todo later
			char gindx[size];
			char count[size];

			bool gindx_parsed = t_snprintf(gindx, size, NULL, "inventory_gindx[%d]", i);		
			bool count_parsed = t_snprintf(count, size, NULL, "inventory_count[%d]", i);	
			
			if(!gindx_parsed || !count_parsed){LOG(LOG_NULL, "Failed to parse inventory data t_snprintf");}
			if(t_check(p.key, gindx)){
				t_atoi(p.value, &e->inventory[i].item_gindx);
				// Set it to not filled
				e->inventory[i].filled = true;
			}
			if(t_check(p.key, count)){
				t_atoi(p.value, &e->inventory[i].count);
				// Set it to not filled
				e->inventory[i].filled = true;
			}
		}
	}
	if(t_check(p.current_section, "hotbar_data")){
		if(e->hotbar_cap== INVALID_CAP){LOG(LOG_NULL, "Hotbar cap is invalid while parsing hotbar data, please place general section before the hotbar data section in the prototype ini"); return;}
		if(!e->hotbar){e->hotbar= XCALLOC(1, sizeof(struct ChildInventorySlot) * e->hotbar_cap);}
		for(int i = 0; i < e->hotbar_cap; i++){
			size_t size = 128;
			char gindx[size];
			char count[size];

			bool gindx_parsed = t_snprintf(gindx, size, NULL, "hotbar_gindx[%d]", i);		
			bool count_parsed = t_snprintf(count, size, NULL, "hotbar_count[%d]", i);	
			
			if(!gindx_parsed || !count_parsed){LOG(LOG_NULL, "Failed to parse hotbar data t_snprintf");}
			if(t_check(p.key, gindx)){
				t_atoi(p.value, &e->hotbar[i].item_gindx);
				// Set it to not filled
				e->hotbar[i].filled = true;
			}
			if(t_check(p.key, count)){
				t_atoi(p.value, &e->hotbar[i].count);
				// Set it to not filled
				e->hotbar[i].filled = true;
			}
		}
	}
	if(t_check(p.current_section, "stats")){
		for(int i = 0; i < BASE_STAT_COUNT; i++){
			char *str = (char *)bsttr(i);

			if(t_check(p.key, str)){
				t_atoi(p.value, &e->bstat_data[i]);
			}
		}	
	}
}
const char *entflgstr(enum EntityFlags flag) {
    switch (flag) {
        #define X(name) case name: return #name;
        FLAG_LIST
        #undef X
        default: return NULL;
    }
}

