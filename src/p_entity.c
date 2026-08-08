#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "p_entity.h"
#include "t_gindex_tool.h"
#include "e_engine_settings.h"
#include "t_config_tool.h"
#include "e_error_handler.h"
#include "t_strings.h"
#include "l_asset_manager.h"

#define ENTITY_CAP 128

// Entitys are mutable, BUT there needs to be a
// string manager since entity starts are immutable and
// will fuck you if you don't free.

static void entity_parser(struct config_pack p, void *ptr);

static struct Entity entities[ENTITY_CAP] = {0};
static struct local_indx iman[ENTITY_CAP] = {0};

static const char *entitdata_lokup[ENTCOM_COUNT] = {
	[E_SPRITE] = "sprite_gindx",
	[E_TEXT] = "text_gindx",
	[E_LOOT] = "loot_gindx",	
	[E_POSX] = "x_pos",
	[E_POSY] = "y_pos",
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

bool e_free_entity(int gindx){
	
	struct AssetFreePackage pckg = {
		.gindx = gindx,
		.index_manager = iman,
		.arr_cap = ENTITY_CAP,
		.arr = entities,
		.element_size = sizeof(struct Entity),
	};
	
	return t_free_asset(pckg);
}

bool e_load_entity(int gindx){
	
	struct AssetLoadPackage pckg = {
		.gindx = gindx,
		.index_manager = iman,
		.arr_cap = ENTITY_CAP,
		.arr = entities,
		.element_size = sizeof(struct Entity),
		.function = entity_parser,
		.path = e_grab_str(ENTITIES_PATH),
		.init = NULL,
	};

	return l_load_asset(pckg);
}
struct Entity e_grab_entity(int gindx, bool autoload){
	int lindx = l_getter_checks(gindx, autoload, ENTITY_CAP, iman, e_load_entity);
	if(!t_indxvalid(ENTITY_CAP, lindx)){ERR_LOG(ERR_FUCKED, "Couldn't load gindx %d", gindx);}
	return entities[lindx];
}
static void entity_parser(struct config_pack p, void *ptr){
	struct Entity *e = (struct Entity*)ptr;
	
	if(!e){ERR_LOG(ERR_FUCKED, "Took null pointer into entity parser.");}	
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
