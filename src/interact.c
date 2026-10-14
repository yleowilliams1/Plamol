#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include "map.h"
#include "util/util.h"
#include "entity.h"
#include "interact.h"
#include "movement.h"
static void update_combat(struct InteractManager *t, struct EntityManager *e, struct Map *m, Vector2 mouse_world_pos);
static void update_normal(struct InteractManager *t, struct EntityManager *e, struct Map *m, Vector2 mouse_world_pos);
static struct Object *find_obj(struct EntityManager *e, Vector2 mouse_world_pos);
struct InteractManager *create_interact(){
	struct InteractManager *m = XCALLOC(1, sizeof(struct InteractManager));
	return m;
}
// This gets called when you do a click
void update_interact(struct InteractManager *t, struct EntityManager *e, struct Map *m, Vector2 mouse_world_pos){
	if(!t || !e || !m){LOG(IS_NULL, "interact, entity, or map are NULL cannot update");return;}	
	if(!e->objects){LOG(IS_NULL, "Entity manager objects is NULl cannot update");return;}
	float map_w = (float)m->width * TILE_W;
	float map_h = (float)m->height * TILE_H;
	if(mouse_world_pos.x < 0 || mouse_world_pos.y < 0 ||mouse_world_pos.x >= map_w || mouse_world_pos.y >= map_h){return;}
	if(e->object_count <= 0){return;}

	if(t->is_combat){update_combat(t, e, m, mouse_world_pos);}
	else{update_normal(t, e, m , mouse_world_pos);}
}
void free_interact(struct InteractManager *m){if(m){free(m); m = NULL;}}
void set_combat_interact(struct InteractManager *m, bool flag){if(m){m->is_combat = flag;}}
static void update_combat(struct InteractManager *t, struct EntityManager *e, struct Map *m, Vector2 mouse_world_pos){
	struct Object *obj = find_obj(e, mouse_world_pos);
	if(obj){
	
	}else{
		vf2 mouse = (vf2){mouse_world_pos.x, mouse_world_pos.y};
		v2 tile = world_to_tile(mouse);
		// 0 is always the player
		move(&e->objects[0].runtime, m, &e->objects[0].save.tx, &e->objects[0].save.ty, (int8_t*)&e->objects[0].save.direction, tile);	
	}
}
static void update_normal(struct InteractManager *t, struct EntityManager *e, struct Map *m, Vector2 mouse_world_pos){		
	struct Object *obj = find_obj(e, mouse_world_pos);
	if(obj){
	
	}else{
		vf2 mouse = (vf2){mouse_world_pos.x, mouse_world_pos.y};
		v2 tile = world_to_tile(mouse);
		// 0 is always the player
		move(&e->objects[0].runtime, m, &e->objects[0].save.tx, &e->objects[0].save.ty, (int8_t*)&e->objects[0].save.direction, tile);	
	}
}
static struct Object *find_obj(struct EntityManager *e, Vector2 mouse_world_pos){
	for(size_t i = 0; i < e->object_count; i++){
		struct Object *obj = &e->objects[i];
		struct ProtoComponent *proto = e->object_prototypes[obj->prototype_index];
		if(!proto){continue;} // prototype not loaded, nothing to click on

		// Object position in world space (tile position plus any sub-tile offset)
		float world_x = obj->save.tx * TILE_W + obj->runtime.tile_offset.x;
		float world_y = obj->save.ty * TILE_H + obj->runtime.tile_offset.y;

		Rectangle box = {
			world_x + proto->rec_offset_x,
			world_y + proto->rec_offset_y,
			proto->rec_width,
			proto->rec_height
		};

		bool hit = CheckCollisionPointRec(mouse_world_pos, box);
		if(hit){
			LOG(INTERACT, "Found entity %d at %d:%d", obj->save.uid, mouse_world_pos.x, mouse_world_pos.y);
			return obj;		
		}
	}

	return NULL;
}
