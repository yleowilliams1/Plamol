#include "c_types.h"
#include "inst_instances.h"
#include "si_world.h"
#include "t_pool.h"
#include "run_runtime.h"

static void run_update_player(struct World *w, struct DouManager *protos, struct InputManager *input, float dt);

struct RuntimeData *run_create_runtime(){
}
void run_update_runtime(struct World *w, struct DouManager *protos, struct InputManager *input, float dt){
	int cursor = 0;
	struct InRef ref;
	struct EntityInstance *e;
	
	// Scan through the available entities
	while((e = t_pool_next(&w->entities, &cursor, &ref))){
		if(e->runtime_flags & (1 << ENT_DEAD)){continue;}
		if(e->current_hp <= 0){si_queue_kill(w, ref);}
		// Skip player
		if(ref.gen == w->player.gen){continue;}
		inst_anim_advance(&e->anim, protos, e->proto_gindx, dt);
	}

	run_update_player(w, protos, input, dt);	
}
void run_draw_runtime(struct World *world);
void run_free_runtime(struct RuntimeData *run);
static void run_update_player(struct World *w, struct DouManager *protos, struct InputManager *input, float dt){
}

