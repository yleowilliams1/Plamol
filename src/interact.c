#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "entity.h"
#include "scene.h"
#include "util/util.h"
#include "interact.h"
#include "settings.h"
#include "escript.h"
#include <stdbool.h>
#include "sprite.h"
#include "movement.h"
static struct InteractModule m[INTERACT_SIZE] = {0};
static bool should_pause = false;
void update_interact(){
	for(int i = 0; i < INTERACT_SIZE; i++){
		if(!m[i].slot_active){continue;}
		if(!m[i].mutable){continue;}
		if(!m[i].immutable){continue;}

		// get_interact_hook_vote() already reported ON_END_INTERACT for this
		// slot once (that's what moved it to CLOSING) - now it's just waiting
		// to be freed. Nothing left to tick.
		if(m[i].phase == INTERACT_PHASE_CLOSING){
			m[i] = (struct InteractModule){0};
			continue;
		}

		m[i].time += GetFrameTime();
		if(is_bit(m[i].mutable->flags, IN_COMBAT)){
			// Combat stuff here. It's empty because i haven't written it yet.
			return;
		}
		if(m[i].immutable->dialogue_path){
			// This is an entity with dialogue.
			// This means we hand off to dialogue manager and zero out this 
		} else if (m[i].immutable->description){
			// Okay so there is a description so floater
			if(m[i].time >= FLT(FLOAT_TIME) && m[i].phase != INTERACT_PHASE_END){
				// Don't free the slot here anymore - flag it as ending so
				// get_interact_hook_vote() gets one shot at ON_END_INTERACT
				// before we tear it down.
				m[i].phase = INTERACT_PHASE_END;
			}
			// This needs to be inverted with drawing since i can't be bothered to write a for loop to inialzie all opacitiyes to 1 like a nerd. Just flip the informaiton is already there
			if(m[i].time >= FLT(FLOAT_TIME)/10){m[i].opacity += 0.1f;}
		} else{
			// This guy doesn't have anything
			LOG(LOAD, "There isn't actually anything wrong, just wanted to point out that we foiund an interactable, but it doens't have anything to show.");
		}
	}
}
enum HookType get_interact_hook_vote(struct EntityMutable *mut){
	if(!mut){return HOOK_COUNT;}
	for(int i = 0; i < INTERACT_SIZE; i++){
		if(!m[i].slot_active){continue;}
		if(m[i].mutable != mut){continue;}
		switch(m[i].phase){
			case INTERACT_PHASE_START:
				m[i].phase = INTERACT_PHASE_IN; // fires exactly once
				return ON_START_INTERACT;
			case INTERACT_PHASE_END:
				m[i].phase = INTERACT_PHASE_CLOSING; // fires exactly once
				return ON_END_INTERACT;
			case INTERACT_PHASE_CLOSING:
				return HOOK_COUNT; // already reported, waiting to be freed
			case INTERACT_PHASE_IN:
			default:
				return ON_IN_INTERACT;
		}
	}
	return HOOK_COUNT; // this entity isn't in an interact slot at all
}
void clear_interact(){
	for(int i = 0; i < INTERACT_SIZE; i++){
		m[i] = (struct InteractModule){0};
	}
}
void interact(struct EntityManager *entity, struct Map *map, vf2 world_position){
	if(!entity){LOG(IS_NULL, "Entity is NULL"); return;}
	if(!map){LOG(IS_NULL, "Map is NULL");return;}
	// This runs every time someone presses the aciton button	
	v2 tile = world_to_tile(world_position);
	if(!in_bounds(map, tile.x ,tile.y)){LOG(IS_NULL, "Out of bounds");return;}

	struct EntityMutable   *target_mut = NULL;
	struct EntityImmutable *target_imm = NULL;

	for(int i = 0; i < INT(ENTITY_INSTANCE_COUNT); i++){
		if(i == 0){continue;} // skip player
		struct EntityMutable *mut = entity->mutable_entity[i];
		if(!mut){continue;}
		if(tile.x != mut->position.x || tile.y != mut->position.y){continue;} 

		struct EntityImmutable *imm = entity->immutable_entity[mut->immutable_gindex];
		if(!imm){continue;}

		// Now do AABB - same tile isn't good enough on its own (a big
		// sprite can visually spill into neighbouring tiles, or have dead
		// space around a smaller hitbox), so check the click actually
		// lands inside this entity's box. imm->x/y is the box's offset
		// from the entity's tile origin, in the same world units as
		// tile_to_world()'s output; imm->width/height is the box size.
		vf2 origin = tile_to_world(mut->position);
		float box_x = origin.x + (float)imm->x;
		float box_y = origin.y + (float)imm->y;
		float box_w = (float)imm->width;
		float box_h = (float)imm->height;

		bool inside = world_position.x >= box_x && world_position.x <= box_x + box_w
		           && world_position.y >= box_y && world_position.y <= box_y + box_h;
		if(!inside){continue;}

		target_mut = mut;
		target_imm = imm;
		break;
	}

	if(!target_mut || !target_imm){
		move(entity->mutable_entity[0], map, tile);
		return;
	} 

	for(int i = 0; i < INTERACT_SIZE; i++){
		if(m[i].slot_active){continue;}
		if(!target_imm->script){continue;}
		if(!is_bit(target_imm->script->hooks_bitmask, ON_START_INTERACT)){continue;}
		if(!is_bit(target_imm->script->hooks_bitmask, ON_IN_INTERACT)){continue;}
		if(!is_bit(target_imm->script->hooks_bitmask, ON_END_INTERACT)){continue;}
		m[i].time += GetFrameTime();	
		
		m[i].slot_active = true;
		m[i].time = 0.0f;
		m[i].opacity = 0.0f;
		m[i].phase = INTERACT_PHASE_START;
		m[i].mutable = target_mut;
		m[i].immutable = target_imm;
		break;
	}
}
void draw_interact(){
}
bool in_paused_interaction(){
	return should_pause;	
}
