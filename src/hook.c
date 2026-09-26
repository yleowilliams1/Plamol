#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "util/util.h"
#include "hook.h"
#include "escript.h"
#include "entity.h"

#define HOOK_PRIORITY_COUNT 64

static struct HookVote votes[HOOK_PRIORITY_COUNT] = {0};

void begin_vote(){
	// Reset votes
	memset(votes, 0, sizeof(votes));
}

void vote_hook(enum HookType vote){
	for(int i = 0; i < HOOK_PRIORITY_COUNT; i++){
		if(votes[i].active) continue;
		votes[i].active = true;
		votes[i].hook = vote;
		return;
	}
	LOG(IS_NULL, "Too many votes, exceeded max %d", HOOK_PRIORITY_COUNT);
}
void end_vote(){
	// Don't actually need to do anything here. this is just for readability
	return;	
}
enum HookType resolve_vote_dispute(void){
	enum HookType best = HOOK_COUNT; // sentinel: higher than any real value
	bool found = false;

	for(int i = 0; i < HOOK_PRIORITY_COUNT; i++){
		if(!votes[i].active) continue;
		if(!found || votes[i].hook < best){
			best = votes[i].hook;
	    		found = true;
		}
	}

	if(!found){
		// This valid. If no votes are sent, then it defaults to idle
		return ON_IDLE; 
	}
	return best;
}
struct Hook *resolve_hook(struct Script *script, enum HookType wanted){
	if(!script){return NULL;}
	if(script->hooks_bitmask & (1u << wanted)){
		return script->hooks[wanted];
	}
	if(script->hooks_bitmask & (1u << ON_IDLE)){
		LOG(IS_NULL, "%d bit wasn't set for script", (int)wanted);
		return script->hooks[ON_IDLE];
	}
	return NULL;
}
