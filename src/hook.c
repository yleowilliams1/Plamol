#include <stdio.h>
#include <stdint.h>
#include "hook.h"
#include "escript.h"


struct Hook *resolve_hook(struct Script *script, enum HookType wanted){
	if(!script){return NULL;}
	if(script->hooks_bitmask & (1u << wanted)){
		return script->hooks[wanted];
	}
	if(script->hooks_bitmask & (1u << ON_IDLE)){
		return script->hooks[ON_IDLE];
	}
	return NULL;
}
