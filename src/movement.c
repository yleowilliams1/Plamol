#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"
#include "entity.h"
#include "scene.h"
#include "input.h"
#include "settings.h"
#include "map.h"
#include "movement.h"
// Here is how this works. When it's time to draw, draw takes the position it has of the entity mutable, then asks movement if it's got anything on it. If it's got an offset from root, t hen apply it.
static struct MovementModule **m = NULL;

// Maps a tile-space step (dx, dy each in {-1,0,1}) to one of the 8 facing
// directions in map.h's Dir enum. Tile coords, not screen coords - the
// isometric projection doesn't change which grid-neighbour we stepped to.
static enum Dir dir_from_delta(int dx, int dy){
	dx = dx < 0 ? -1 : (dx > 0 ? 1 : 0);
	dy = dy < 0 ? -1 : (dy > 0 ? 1 : 0);
	if(dx == -1 && dy == -1){return NW;}
	if(dx == -1 && dy ==  1){return SW;}
	if(dx ==  1 && dy ==  1){return SE;}
	if(dx ==  1 && dy == -1){return NE;}
	if(dx ==  0 && dy == -1){return N;}
	if(dx == -1 && dy ==  0){return W;}
	if(dx ==  0 && dy ==  1){return S;}
	if(dx ==  1 && dy ==  0){return E;}
	return S; // dx == 0 && dy == 0 - shouldn't happen for a real path segment
}
// Faces the entity toward whatever segment (root_tile -> next_tile) it's
// currently walking. Called whenever that pair changes: once when a new
// path starts, and again each time update_move() advances to the next tile.
static void face_current_segment(struct MovementModule *mod){
	struct PathNode root = mod->path[mod->root_tile];
	struct PathNode next = mod->path[mod->next_tile];
	mod->entity->direction = dir_from_delta(next.x - root.x, next.y - root.y);
}

void init_move(){
	if(m){return;}
	m = XCALLOC(1, sizeof(struct MovementModule *) * INT(ENTITY_INSTANCE_COUNT));
}
void free_move(){
	if(!m){return;}
	clear_move();
	free(m);
	m = NULL;
}
void clear_move(){
	if(!m){return;}
	for(int i = 0; i < INT(ENTITY_INSTANCE_COUNT); i++){
		if(!m[i]){continue;}
		if(!m[i]->path){continue;}
		free(m[i]->path);
		free(m[i]);
		m[i] = NULL;
	}
}
void update_move(){
	if(!m){return;}
	for(int i = 0; i < INT(ENTITY_INSTANCE_COUNT); i++){
		if(!m[i]){continue;}
		if(!m[i]->path){continue;}

		struct PathNode root = m[i]->path[m[i]->root_tile];
		struct PathNode next = m[i]->path[m[i]->next_tile];
		vf2 root_world = tile_to_world((v2){root.x, root.y});
		vf2 next_world = tile_to_world((v2){next.x, next.y});

		// lerp (in world space, so this is correct regardless of how
		// tile_to_world scales/projects tile coords)
		float t = m[i]->time / FLT(SECONDS_PER_TILE);
		m[i]->offset_from_root.x = (next_world.x - root_world.x) * t;
		m[i]->offset_from_root.y = (next_world.y - root_world.y) * t;

		m[i]->time += GetFrameTime();
		if(m[i]->time < FLT(SECONDS_PER_TILE)){continue;}
		m[i]->time = 0.0f;

		// advance to the next tile in the path
		m[i]->root_tile = m[i]->next_tile;
		m[i]->entity->position.x = m[i]->path[m[i]->root_tile].x;
		m[i]->entity->position.y = m[i]->path[m[i]->root_tile].y;
		m[i]->next_tile++;
		if((size_t)m[i]->next_tile >= m[i]->len){
			// reached the end of the path
			free(m[i]->path);
			free(m[i]);
			m[i] = NULL;
			continue;
		}
		face_current_segment(m[i]);
	}
}
void move(struct EntityMutable *e, struct Map *map, v2 end){
	if(!map){return;}
	if(!e){return;}
	v2 start = {e->position.x, e->position.y};
	if(!in_bounds(map, start.x, start.y)){return;}
	if(!in_bounds(map, end.x, end.y)){return;}
	if(!m){return;}
	struct MovementModule **mod = &m[e->mutable_gindex];
	if(*mod){free((*mod)->path); free(*mod); *mod = NULL;}
	*mod = XCALLOC(1, sizeof(struct MovementModule));

	bool found = astar_find_path(map, start, end, &((*mod)->path), &((*mod)->len));
	if(!found){
		free(*mod);
		*mod = NULL;
		return;
	}
	if((*mod)->len < 2){
		// already at (or adjacent-collapsed onto) the target tile, nothing to animate
		free((*mod)->path);
		free(*mod);
		*mod = NULL;
		return;
	}
	(*mod)->entity = e;
	(*mod)->root_tile = 0;
	(*mod)->next_tile = 1;
	face_current_segment(*mod);
}
bool get_move_offset(int indx, vf2 *out){
	if(!m){return false;}
	if(!out){return false;}
	if(indx > INT(ENTITY_INSTANCE_COUNT)){return false;}
	if(!m[indx]){return false;}
	if(!m[indx]->path){return false;}
	*out = m[indx]->offset_from_root;
	return true;
}
bool is_moving(int indx){
	if(!m){return false;}
	if(indx > INT(ENTITY_INSTANCE_COUNT)){return false;}
	if(!m[indx]){return false;}
	if(!m[indx]->path){return false;}
	return true;
}
