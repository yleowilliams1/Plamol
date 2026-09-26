#pragma once
#include "util/util.h"
#include "entity.h"
#include "hook.h"
#include <stdbool.h>
#define INTERACT_SIZE 128

// Internal progress of one interact slot. Doesn't map 1:1 onto HookType on
// purpose - START/END are each only reported as a vote once (see
// get_interact_hook_vote), then we move straight past them so the caller
// never sees the same edge twice. CLOSING just means "already reported
// ON_END_INTERACT, waiting for update_interact() to free the slot".
enum InteractPhase{
	INTERACT_PHASE_START = 0,
	INTERACT_PHASE_IN,
	INTERACT_PHASE_END,
	INTERACT_PHASE_CLOSING,
};
struct InteractModule{
	float time;
	float opacity;		
	bool slot_active;
	enum InteractPhase phase;
	struct EntityImmutable *immutable;
	struct EntityMutable *mutable;	
};

void update_interact();
void clear_interact();
void interact(struct EntityManager *entity, struct Map *map, vf2 world_position);
void draw_interact();
bool in_paused_interaction();
// The single place that decides what hook (if any) an entity's interact
// state should vote for this frame. Returns HOOK_COUNT if this entity
// isn't in an interact slot at all. Reading ON_START_INTERACT or
// ON_END_INTERACT here consumes that edge - it only ever comes back once
// per interact - so simulate_entities() can just call this every frame
// per entity without tracking any interact-specific state itself.
enum HookType get_interact_hook_vote(struct EntityMutable *mut);
