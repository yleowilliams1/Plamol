#include <stdlib.h>
#include "ai.h"
#include "entity.h"
#include "escript.h"
#include "hook.h"
#include "map.h"
#include "scene.h"
#include "sprite.h"
#include "util/util.h"

/*
 * decide_hook() looks at an entity's current state (flags, stats,
 * position, whatever else you need) and returns the HookType that
 * should be active for it this frame. It does NOT do anything to that
 * state itself (no moving, no attacking) - that's escript/apply_script
 * and the hook's own flag_add/flag_remove config. This function's only
 * job is to answer "given what this NPC is right now, which hook is it
 * in?"
 *
 * Return mut->active_hook unchanged to stay in the current hook.
 */
static enum HookType decide_hook(struct Scene *scene, struct Entity *e, struct EntityImmutable *imm, struct EntityMutable *mut){
	// TODO: your actual AI decision making goes here. e.g.:
	//   if(mut->current_health_points <= 0){return ON_DEATH;}
	//   if(<hostile and can see/reach the player>){return ON_MOVING;}
	//   if(<player started talking to this entity>){return ON_START_INTERACT;}
	return mut->active_hook;
}

void simulate_entities(struct Scene *scene){
	if(!scene || !scene->map || !scene->entity_manager){return;}
	struct Map *map = scene->map;
	struct EntityManager *eman = scene->entity_manager;

	for(int i=0; i<map->entity_count; i++){
		struct Entity *e = &map->entity[i];
		struct EntityImmutable *imm = eman->immutable_entity[e->prototype_gindex];
		struct EntityMutable   *mut = eman->mutable_entity[e->instance_gindex];
		if(!imm || !mut){continue;}
	
		// 0 is always the player
		if(i == 0){continue;}

		enum HookType current = mut->active_hook;
		enum HookType next = decide_hook(scene, e, imm, mut);

		if(next >= HOOK_COUNT){
			LOG(INDEX, "decide_hook() returned invalid hook %d for instance %d - ignoring", next, e->instance_gindex);
			next = current;
		}

		if(next != current){
			mut->active_hook = next;
			// New hook -> new animation, start it from frame 0.
			mut->current_frame = 0;
			mut->elapsed_time = 0.0f;
		}

		// Applies this hook's flag_add/flag_remove and advances
		// current_frame - always run last, after the hook above settles.
		apply_script(mut, imm, scene->sprites_manager);
	}
}
