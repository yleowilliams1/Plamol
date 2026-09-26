#include <stdlib.h>
#include "simulate.h"
#include "entity.h"
#include "escript.h"
#include "hook.h"
#include "interact.h"
#include "movement.h"
#include "map.h"
#include "scene.h"
#include "sprite.h"
#include "util/util.h"

/*
 * This is the one place in the whole engine that's allowed to change
 * mut->active_hook. Every subsystem that has an opinion about what an
 * entity should be doing this frame (movement, interact, death, combat,
 * eventually player input) just calls vote_hook() instead of writing
 * active_hook directly. We collect everyone's vote, resolve_vote_dispute()
 * picks a winner by HOOKS_LIST priority order, and only THEN do we touch
 * active_hook - and reset the animation if (and only if) it actually
 * changed. No other file should be assigning to active_hook,
 * current_frame or elapsed_time anymore.
 */
static void cast_votes(struct Scene *scene, struct Entity *e, struct EntityImmutable *imm, struct EntityMutable *mut){
	// Death always wins by virtue of ON_DEATH sitting early in HOOKS_LIST,
	// but flag it here too since nothing else in the engine sets DEAD yet.
	if(mut->current_health_points <= 0){
		mut->flags |= (1u << DEAD);
		vote_hook(ON_DEATH);
	}

	if(is_moving(mut->mutable_gindex)){
		vote_hook(ON_MOVING);
	}

	enum HookType interact_vote = get_interact_hook_vote(mut);
	if(interact_vote != HOOK_COUNT){
		vote_hook(interact_vote);
	}

	// --- Combat hooks: stubs. combat.c doesn't produce any of this state
	// yet, so there's nothing real to check - these are just placeholders
	// so the wiring exists and you don't have to come back and rewire the
	// voting loop once combat exists. Uncomment/replace the condition as
	// each piece gets built.
	// if(<this entity was hit by an attack this frame>)  vote_hook(ON_DAMAGED);
	// if(<an attack against this entity missed>)         vote_hook(ON_MISSED);
	// if(<it's this entity's turn to act>)                vote_hook(ON_TURN_START);
	// if(<this entity just finished its turn>)            vote_hook(ON_TURN_END);
	// if(<this entity fired a pistol this frame>)         vote_hook(ON_SHOOT_PISTOL);
	// if(<... submachine gun>)                             vote_hook(ON_SHOOT_SMG);
	// if(<... rifle>)                                      vote_hook(ON_SHOOT_RIFLE);
	// if(<... knife attack>)                               vote_hook(ON_ATTACK_KNIFE);
	// if(<... sledgehammer attack>)                        vote_hook(ON_ATTACK_SLEDGEHAMMER);
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

		enum HookType current = mut->active_hook;

		begin_vote();
		cast_votes(scene, e, imm, mut);
		end_vote();
		enum HookType next = resolve_vote_dispute();

		if(next >= HOOK_COUNT){
			LOG(INDEX, "resolve_vote_dispute() returned invalid hook %d for instance %d - ignoring", next, e->instance_gindex);
			next = current;
		}

		if(next != current){
			mut->active_hook = next;
			// The ONE spot animations get reset - only when the winning
			// hook actually differs from what we were just in, so we never
			// end up indexing into a new animation with current_frame left
			// over from a hook that had more frames than this one does.
			mut->current_frame = 0;
			mut->elapsed_time = 0.0f;
		}

		// Applies this hook's flag_add/flag_remove and advances
		// current_frame - always run last, after the hook above settles.
		apply_script(mut, imm, scene->sprites_manager);
	}
}
