#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"
#include "entity.h"
#include "player.h"
#include "scene.h"
#include "input.h"
#include "movement.h"
void simulate_player(struct Scene *scene){
	if(!scene){return;}
	if(!scene->entity_manager){return;}
	if(!scene->map){return;}
	// Immutable and mutable index 0 is always player
	struct EntityMutable *player = scene->entity_manager->mutable_entity[0];	

	if(!player){LOG(IS_NULL, "No player"); return;}
	if(is_moving(0)){player->active_hook = ON_MOVING;}
	else{player->active_hook = ON_IDLE;}

	if(pressed(ACTION)){
		Vector2 world_pos = GetScreenToWorld2D(GetMousePosition(), scene->camera);
		vf2 w = {world_pos.x, world_pos.y};
		v2 t = world_to_tile(w);
		if(t.x < 0 && t.y > scene->map->width && t.y < 0 && t.y > scene->map->height){LOG(IS_NULL, "Out of bounds"); return;}
		move(player, scene->map, t);	
	}
}
void draw_player_ui(struct Scene *scene){
	return;
}
