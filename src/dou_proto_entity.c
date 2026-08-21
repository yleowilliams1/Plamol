#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "dou_proto_entity.h"

#include "t_log_handler.h"
#include "t_strings.h"

#include "c_magic_number.h"
#include "c_data_enums.h"
#include "c_flag_enums.h"
#include "c_dou_struct_defs.h"

#include "e_prototype_manager.h"

static void entity_on_free(void *slot);
static void entity_on_init(void *slot);
static void entity_on_ploader(void *slot);
static void entity_on_loader(struct config_pack p, void *ptr);

struct ItemFunctions prototype_entity(){
	return (struct ItemFunctions){
		.on_load = entity_on_loader,
		.on_init = entity_on_init,
		.on_free = entity_on_free,
		.on_pload = entity_on_ploader,
	};
}

static void entity_on_free(void *slot){
	struct DouEntityPrototype *e = (struct DouEntityPrototype*)slot;	
	if(!e){LOG(LOG_NULL, "Took null pointer into dou entity prototype free."); return;}	

	LOG(LOG_FREE, "Freeing dou entity prototype of address %p", slot);
}
static void entity_on_init(void *slot){
	struct DouEntityPrototype *e = (struct DouEntityPrototype*)slot;	
	if(!e){LOG(LOG_NULL, "Took null pointer into dou entity prototype init."); return;}	
	
	LOG(LOG_LOAD, "Initalizing dou entity prototype of address %p", slot);
}
static void entity_on_ploader(void *slot){
	struct DouEntityPrototype *e = (struct DouEntityPrototype*)slot;	
	if(!e){LOG(LOG_NULL, "Took null pointer into dou entity prototype ploader."); return;}	
	

	LOG(LOG_LOAD, "Post-Loading dou entity prototype of address %p", slot);
}
static void entity_on_loader(struct config_pack p, void *ptr){
	struct DouEntityPrototype *e = (struct DouEntityPrototype*)ptr;	
	if(!e){LOG(LOG_NULL, "Took null pointer into dou entity prototype parser."); return;}	
	
	if(t_check(p.current_section, "general")){
		for(int i = 0; i < ENTITY_DATA_COUNT; i++){
			char *str = edata_str(i);
			if(!t_check(p.key, str)){continue;}
			t_atoi(p.value, &e->entity_data[i]);
		}	
	}
	if(t_check(p.current_section, "flags")){
		for(int i = 0; i < ENTITY_FLAG_COUNT; i++){
			char *str = entity_flag_str(i);
			if(!t_check(p.key, str)){continue;}
			int value;
			t_atoi(p.value, &value);
			if(value > 0){e->flags |= (1<< i);}
		}
	}
	if(t_check(p.current_section, "inventory_data")){
		for(int i = 0; i < MAX_INV_SIZE; i++){
			size_t size = 32;
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
		for(int i = 0; i < MAX_HOTBAR_SIZE; i++){
			size_t size = 32;
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
			char *str = bstat_str(i);

			if(t_check(p.key, str)){
				t_atoi(p.value, &e->bstats_data[i]);
			}
		}	
	}
}
