#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"
#include "entity.h"
#include "settings.h"
#include "map.h"
#include "movement.h"
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
static void face_current_segment(struct RuntimeComponent *mod, int8_t *dir){
	if(!mod || !dir){return;}
	struct PathNode root = mod->path[mod->root_tile];
	struct PathNode next = mod->path[mod->next_tile];
	*dir = dir_from_delta(next.x - root.x, next.y - root.y);
}
void update_move(struct RuntimeComponent *m, int32_t *tx, int32_t *ty, int8_t *dir){
	if(!m || !tx || !ty){return;}
	if(!m->path){return;}

	struct PathNode root = m->path[m->root_tile];
	struct PathNode next = m->path[m->next_tile];
	vf2 root_world = tile_to_world((v2){root.x, root.y});
	vf2 next_world = tile_to_world((v2){next.x, next.y});

	// lerp (in world space, so this is correct regardless of how
	// tile_to_world scales/projects tile coords)
	float t = m->time / FLT(SECONDS_PER_TILE);
	m->offset_from_root.x = (next_world.x - root_world.x) * t;
	m->offset_from_root.y = (next_world.y - root_world.y) * t;

	m->time += GetFrameTime();
	if(m->time < FLT(SECONDS_PER_TILE)){return;}
	m->time = 0.0f;

	// advance to the next tile in the path
	m->root_tile = m->next_tile;
	*tx = m->path[m->root_tile].x;
	*ty = m->path[m->root_tile].y;
	m->next_tile++;
	
	if((size_t)m->next_tile >= m->len){
		// reached the end of the path
		free(m->path);
		return;
	}
	face_current_segment(m, dir);
}
void move(struct RuntimeComponent *m, struct Map *map, int32_t *tx, int32_t *ty, int8_t *dir, v2 end){
	if(!map || !m || !tx || !ty){return;}
	
	v2 start = {*tx, *ty};
	if(!in_bounds(map, start.x, start.y)){return;}
	if(!in_bounds(map, end.x, end.y)){return;}

	if(m->path){free(m->path); m->path = NULL;}
	m->len = 0;
	m->time = 0.0f;
	m->root_tile = 0;
	m->next_tile = 0;
	m->offset_from_root = (vf2){0};
	

	bool found = astar_find_path(map, start, end, &m->path, &m->len);
	if(!found){return;}
	if(m->len < 2){
		// already at (or adjacent-collapsed onto) the target tile, nothing to animate
		if(m->path){free(m->path); m->path = NULL;}
		return;
	}
	m->root_tile = 0;
	m->next_tile = 1;
	face_current_segment(m, dir);
}
bool get_move_offset(struct RuntimeComponent *m, vf2 *out){
	if(!m){return false;}
	if(!out){return false;}
	if(!m->path){return false;}
	*out = m->offset_from_root;
	return true;
}
bool is_moving(struct RuntimeComponent *m){
	if(!m){return false;}
	if(!m->path){return false;}
	return true;
}
