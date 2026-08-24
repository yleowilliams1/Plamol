#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "depo_entity.h"

#include "t_log_handler.h"
#include "t_strings.h"
#include "t_depot_manager.h"
#include "t_config_tool.h"

#include "c_magic_number.h"

static void prototype_on_free(void *slot);
static void prototype_on_init(void *slot);
static void prototype_on_ploader(void *slot);
static void prototype_on_loader(struct config_pack p, void *ptr);
static void prototype_on_interact(void *interactdata, void *slot);

static void instance_on_free(void *slot);
static void instance_on_init(void *slot);
static void instance_on_ploader(void *slot);
static void instance_on_load(struct config_pack p, void *ptr);
static void instance_on_bulk(void *loaddata, void *ptr);
static void instance_on_interact(void *interactdata, void *slot);

struct ItemFunctions entity_prototype(){
	return (struct ItemFunctions){
		.on_load = prototype_on_loader,
		.on_bulk = NULL,
		.on_init = prototype_on_init,
		.on_free = prototype_on_free,
		.on_pload = prototype_on_ploader,
		.on_interact = prototype_on_interact,
	};
}

struct ItemFunctions entity_instance(){
	return (struct ItemFunctions){
		.on_load = instance_on_load,
		.on_bulk = instance_on_bulk,
		.on_init = instance_on_init,
		.on_free = instance_on_free,
		.on_pload = instance_on_ploader,
		.on_interact = instance_on_interact,
	};
}

// Think abotu a solution to type safety later
static void instance_on_free(void *slot){
	struct EntityInstance *e = (struct EntityInstance *)slot;
	if(!e){LOG(LOG_NULL, "Entity instance is NULL");return;}
	if(e->inventory){free(e->inventory); e->inventory = NULL;}
	if(e->hotbar){free(e->hotbar); e->hotbar = NULL;}
}
static void instance_on_init(void *slot){
	struct EntityInstance *e = (struct EntityInstance *)slot;
	if(!e){LOG(LOG_NULL, "Entity instance is NULL");return;}
}
static void instance_on_ploader(void *slot){
	struct EntityInstance *e = (struct EntityInstance *)slot;
	if(!e){LOG(LOG_NULL, "Entity instance is NULL");return;}
}
static void instance_on_load(struct config_pack p, void *ptr){
	// Load save data here and figure out runtime data like health and ap
}
static void instance_on_bulk(void *loaddata, void *ptr){
	struct EntityInstance *e = (struct EntityInstance *)ptr;
	if(!e){LOG(LOG_NULL, "Entity instance is NULL");return;}
	struct EntityInstanceLoadData *ld = (struct EntityInstanceLoadData *)loaddata;
	if(!ld){LOG(LOG_NULL, "Load data is NULL");return;}
	struct EntityPrototype *prototype = (ld->prototype);
	if(!prototype){LOG(LOG_NULL, "Prototype is NULL");return;}
	
	e->tile = ld->start_tile;
	e->facing = ld->start_direction;
	e->runtime_flags = prototype->flags;

	e->inventory_size = prototype->inventory_cap;
	e->hotbar_size = prototype->hotbar_cap;
	
	if(!e->inventory){e->inventory = XCALLOC(1, sizeof(struct ChildInventorySlot) * e->inventory_size);}
	if(!e->hotbar){e->hotbar = XCALLOC(1, sizeof(struct ChildInventorySlot) * e->hotbar_size);}

	if(prototype->inventory){*e->inventory = *prototype->inventory;}
	if(prototype->hotbar){*e->hotbar = *prototype->hotbar;}
	
	memcpy(e->bstat, prototype->bstat_data, sizeof(e->bstat));
}
static void instance_on_interact(void *interactdata, void *slot){
	struct EntityInstance *e = (struct EntityInstance *)slot;
	if(!e){LOG(LOG_NULL, "Entity instance is NULL");return;}
	struct EntityInteractData *ld = (struct EntityInteractData *)interactdata;
	if(!ld){LOG(LOG_NULL, "Load data is NULL");return;}

	LOG(LOG_LOAD, "Hello. I have been interact with! Hipee...");	
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
			char *str = (char *)bstatstr(i);

			if(t_check(p.key, str)){
				t_atoi(p.value, &e->bstat_data[i]);
			}
		}	
	}
}
static void prototype_on_interact(void *interactdata, void *slot){
	LOG(LOG_NULL, "Tried to interact with prototype! Returning");
	return;
}
const char *entflgstr(enum EntityFlags flag) {
    switch (flag) {
        #define X(name) case name: return #name;
        FLAG_LIST
        #undef X
        default: return NULL;
    }
}

const char *bstatstr(enum BaseStatEnum stat){
    switch (stat) {
        #define X(name) case name: return #name;
        BSTAT_LIST
        #undef X
        default: return NULL;
    }
}
const char *dstatstr(enum DerivedStatEnum stat){
    switch (stat) {
	#define X(name) case name: return #name;
	DSTAT_LIST
	#undef X
	default: return NULL;
    }
}
