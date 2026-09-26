#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"
#include "entity.h"
#include "player.h"
#include "scene.h"
#include "input.h"
#include "interact.h"
void simulate_player(struct Scene *scene){
	if(!scene){return;}
	if(!scene->entity_manager){return;}
	if(!scene->map){return;}
	// Immutable and mutable index 0 is always player
	struct EntityMutable *player = scene->entity_manager->mutable_entity[0];	
	struct EntityImmutable *player_collision = scene->entity_manager->immutable_entity[0];

	if(!player || !player_collision){/*LOG(IS_NULL, "No player");*/ return;}

	if(pressed(ACTION)){
		Vector2 world_pos = GetScreenToWorld2D(GetMousePosition(), scene->camera);
		vf2 w = {world_pos.x, world_pos.y};
		interact(scene->entity_manager, scene->map, w);	
	}
}
void draw_player_ui(struct Scene *scene){
	draw_interact();
	return;
}
