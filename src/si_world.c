#include <stdio.h>
#include <string.h>
#include "t_pool.h"
#include "c_magic_number.h"
#include "c_types.h"
#include "e_dou_manager.h"
#include "t_math.h"
#include "e_engine_settings.h"
#include "si_map.h"
#include "t_log_handler.h"
#include "si_world.h"
#include "inst_instances.h"
#include "run_runtime.h"

// This doesn't have interactable support yet

static struct InRef si_spawn_entity(struct World *w, struct DouManager *protos,int proto_gindx, v3 tile, int guid, enum TileDirections facing);

static void si_occupy_set(struct World *w, v3 tile, struct InRef ref);
static void si_occupy_clear(struct World *w, v3 tile);

static void si_flush_kills(struct World *w);

struct World *si_load_world(struct DouManager *protos, int map_gindx){
	struct World *w = XCALLOC(1, sizeof(struct World));
	w->map = si_load_map(map_gindx);
	if(!w->map){free(w); LOG(LOG_NULL, "Failed to load map gindx %d", map_gindx);return NULL;}
	t_pool_init(&w->entities ,e_grab_inscount(INSTANCE_ENTITY), sizeof(struct EntityInstance));
	t_pool_init(&w->interactables,e_grab_inscount(INSTANCE_INTERACTABLE), sizeof(struct InteractableInstance));
	w->runtime = run_create_runtime();
	if(w->runtime == NULL){LOG(LOG_NULL, "Runtime failed to load");}
	int tiles = w->map->metadata[M_WIDTH] * w->map->metadata[M_HEIGHT];
	w->occupancy = XCALLOC(1, sizeof(struct InRef) * tiles);

	for(int i = 0; i < w->map->metadata[M_ENTITY_COUNT]; i++){
	        struct MapEntityData *e = &w->map->entities[i];
		v3 pos = {.x = e->tile_spawn_x, .y = e->tile_spawn_y, .z = 0 };
        	si_spawn_entity(w, protos, e->gindx, pos, e->GUID, e->dir);
	}
	return w;
}
void si_free_world(struct World *w){
	bool map_freed = si_free_map(w->map);
	w->map = NULL;
	if(!map_freed){LOG(LOG_RELOAD, "Failed to free map!");}
	t_pool_free_all(&w->entities);
	t_pool_free_all(&w->interactables);
	run_free_runtime(w->runtime);
	free(w->occupancy);
	w->occupancy = NULL;
	free(w);
	w = NULL;
}
void si_update_world(struct World *w, struct DouManager *protos, float dt, struct InputManager *input){
    	run_update_runtime(w, protos, input, dt);
    	si_flush_kills(w);
}
void si_draw_world(struct World *w){
	run_draw_runtime(w);
}
bool si_try_move(struct World *w, struct InRef ref, v3 to){
    	struct EntityInstance *e = t_pool_get(&w->entities, ref);
   	if(!e){ return false; }

    	int idx = to.y * w->map->metadata[M_WIDTH] + to.x;
    	struct MapDecompTile *t = &w->map->tiles[idx];

    	if(t->flags & (1 << T_WALL_COLLIDE)){ return false; }
    	if(t_pool_get(&w->entities, w->occupancy[idx])){ return false; }  // occupied

    	si_occupy_clear(w, e->tile);
    	e->tile = to;
    	si_occupy_set(w, to, ref);
    	return true;
}
static struct InRef si_spawn_entity(struct World *w, struct DouManager *protos,int proto_gindx, v3 tile, int guid, enum TileDirections facing){
	struct InRef ref;
	struct EntityInstance *e = t_pool_alloc(&w->entities, &ref);
	if(!e){ return (struct InRef){0}; }	

	struct Entity *p = (struct Entity *)e_dou_get(protos, proto_gindx, DOU_ENTIT);
	if(!p){ t_pool_release(&w->entities, ref); return (struct InRef){0}; }

	e->proto_gindx   = proto_gindx;
	e->guid          = guid;
	e->tile          = tile;
	e->facing        = facing;
	// The enum has to be shared for this btw
	e->runtime_flags = p->flags;

	if(p->flags & (1 << ENT_STAT)){
		struct BaseStats *bs = (struct BaseStats *)e_dou_get(protos, p->data[E_STAT], DOU_STAT);
		if(bs){ memcpy(e->stats.base, bs->basestats, sizeof(e->stats.base)); }
	}
	if(p->flags & (1 << ENT_INV)){
		struct Inventory *iv = (struct Inventory *)e_dou_get(protos, p->data[E_INV], DOU_INV);
		if(iv){ memcpy(&e->inv, iv, sizeof(e->inv));}
	}

	e->current_hp = inst_derive_stat(&e->stats, MAX_HP);
	e->current_ap = 100;

	si_occupy_set(w, tile, ref);

	return ref;
}

static void si_occupy_set(struct World *w, v3 tile, struct InRef ref){
	int idx = tile.y * w->map->metadata[M_WIDTH] + tile.x;
	w->occupancy[idx] = ref; 
}
static void si_occupy_clear(struct World *w, v3 tile){
	int idx = tile.y * w->map->metadata[M_WIDTH] + tile.x;
	w->occupancy[idx] = (struct InRef){0}; 
}
void si_queue_kill(struct World *w, struct InRef r){
    if(w->kill_count < MAX_KILLS_PER_FRAME){ w->kill_queue[w->kill_count++] = r; }
}

static void si_flush_kills(struct World *w){
    for(int i = 0; i < w->kill_count; i++){
        struct EntityInstance *e = t_pool_get(&w->entities, w->kill_queue[i]);
        if(!e){ continue; }              // already gone; double-queue is harmless
        si_occupy_clear(w, e->tile);
        t_pool_release(&w->entities, w->kill_queue[i]);
    }
    w->kill_count = 0;
}

