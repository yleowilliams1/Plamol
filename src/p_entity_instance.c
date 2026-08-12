#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>
#include "e_error_handler.h"
#include "t_math.h"
#include "p_entity_instance.h"
#include "t_strings.h"
#include "m_render_map.h"
#include "l_sprite_manager.h"
#include "m_map.h"

// I gotta go through this and clean alot of this up
#define SEARCH_SIZE 256

void e_update_entities(struct MapPack *map, int *pz_out, struct EntityInstance *e){
	for(int i = 0; i < SEARCH_SIZE; i++){
		
	}	
}

bool e_consume_item(struct EntityInstance *e){
	ERR_LOG(ERR_OK, "Consumed item");	
	return true;
}
bool e_move_entity(struct EntityInstance *e, vf2 norm_input, float speed, float delta, bool can_move){
	if(!can_move){return false;}
	e->e.data[E_POSX] += (norm_input.x * speed * delta);
	e->e.data[E_POSY] += (norm_input.y * speed * delta);
	return true;
}
bool e_mod_health(struct EntityInstance *e, int amount){
	return true;
}
bool e_grab_dstat(struct EntityInstance *e, enum Dev dstat, int *out){
	return true;
}
bool e_grab_bstat(struct EntityInstance *e, enum Stats bstat, int *out){
	return true;
}
bool e_grab_loot(struct EntityInstance *e, struct LootData *out){
	return true;
}
