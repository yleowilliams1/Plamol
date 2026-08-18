#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dou_proto_entity.h"
#include "t_config_tool.h"
#include "t_log_handler.h"
#include "t_strings.h"
#include "c_magic_number.h"
#include "e_dou_manager.h"

static void entity_on_free(void *slot);
static void entity_on_init(void *slot);
static void entity_on_ploader(void *slot);
static void entity_on_loader(struct config_pack p, void *ptr);

static const char *entitdata_lokup[ENTCOM_COUNT] = {
	[E_SPRITE] = "sprite_gindx",
	[E_TEXT] = "text_gindx",
	[E_LOOT] = "loot_gindx",	
	[E_STAT] = "stat_gindx",
	[E_INV] = "inventory_gindx",
};

static const char *entitflag_lokup[ENTITY_FLAG_COUNT] = {
	[ENT_COMBAT] = "combat_flag",
	[ENT_SPRITE] = "sprite_flag",
	[ENT_TEXT] = "text_flag",
	[ENT_LOOT] = "loot_flag",
	[ENT_PLAYER] = "playable",
	[ENT_HOSTILE] = "hostile",
	[ENT_STAT] = "stat_flag",
	[ENT_INV] = "inventory_flag",
};

struct DouLoader dou_entity(){
	return (struct DouLoader){
		.on_load = entity_on_loader,
		.on_init = entity_on_init,
		.on_free = entity_on_free,
		.on_pload = entity_on_ploader,
		.size = sizeof(struct Entity),
		.type = DOU_ENTIT,
	};
}

static void entity_on_free(void *slot){
	LOG(LOG_FREE, "Freeing entity of address %p", slot);
}
static void entity_on_init(void *slot){
	LOG(LOG_LOAD, "Initalizing entity of address %p", slot);
}
static void entity_on_ploader(void *slot){
	LOG(LOG_LOAD, "Post-Loading entity of address %p", slot);
}
static void entity_on_loader(struct config_pack p, void *ptr){
	struct Entity *e = (struct Entity*)ptr;
	
	if(!e){LOG(LOG_ABORT, "Took null pointer into entity parser.");}	
	if(t_check(p.current_section, "general")){
		for(int i = 0; i < ENTCOM_COUNT; i++){
			char *str = (char *)entitdata_lokup[i];
			if(!t_check(p.key, str)){continue;}
			t_atoi(p.value, &e->data[i]);
		}	
	}
	if(t_check(p.current_section, "flags")){
		for(int i = 0; i < ENTITY_FLAG_COUNT; i++){
			char *str = (char*)entitflag_lokup[i];
			if(!t_check(p.key, str)){continue;}
			int value;
			t_atoi(p.value, &value);
			if(value > 0){e->flags |= (1<< i);}
		}
	}
}
