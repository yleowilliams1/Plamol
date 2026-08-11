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

void e_draw_entity_pool(struct EntityInstance *pool, int size){
	if(!pool){return;}
	// Optimize later
	for(int i = 0; i < size; i++){
		if(!pool[i].valid){continue;}
		// E_POSX/E_POSY are TILE coordinates. Using them directly as world
		// coordinates put every entity in a sub-pixel clump at the origin.
		Vector2 pos = m_tile_to_world(pool[i].e.data[E_POSX], pool[i].e.data[E_POSY], 0);
		if(pool[i].e.flags & (1 << ENT_SPRITE)){
			l_draw_sprite(pool[i].e.data[E_SPRITE], true, pos, 0, 0);
		} else {
			DrawCircleV(pos, 6.0f, RED);
		}
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
