#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include "util/util.h"
#include "escript.h"
#include "entity.h"
#include "sprite.h"
#include "settings.h"
#include "hook.h"
// Config format assumed (adjust if yours differs):
//
//   flags=PLAYER|HOSTILE          <- lines before any [SECTION] set script-wide flags
//
//   [ON_IDLE]
//   frame_count=8
//   animation=0
//   play_and_hold=false
//   flag_add=PLAYER|COLLIDE
//   flag_remove=LOCKED
//
// Each [SECTION] name must match one of the HOOKS_LIST entries. flag_add/
// flag_remove/flags accept one or more HOOK_FLAGS names separated by '|'.

// Turns "PLAYER|COLLIDE" into the OR'd bitmask of those HOOK_FLAGS values.
void apply_script(struct EntityMutable *entity_mutable, struct EntityImmutable *entity_immutable, struct SpriteManager *sprite){
	if(!entity_mutable){LOG(IS_NULL, "entity mutable is NULL"); return;}	
	if(!entity_immutable){LOG(IS_NULL, "entity immutable is NULL");return;}
	struct Script *script = entity_immutable->script;
	if(!script){LOG(IS_NULL, "Script is NULL"); return;}

	if(!sprite){LOG(IS_NULL, "Spritemanager is NULL"); return;}

	struct Hook *h = resolve_hook(script, entity_mutable->active_hook);
	if(!h){return;}

	entity_mutable->flags |= h->flag_add;
	entity_mutable->flags &= ~h->flag_remove;

	struct SpriteData *sprite_data = sprite->sprite[entity_immutable->sprite_gindex]; 
	if(!sprite_data){LOG(IS_NULL, "%d sprite for entity immutable %d is NULL", entity_immutable->sprite_gindex, entity_immutable->immutable_gindex); return;}

	if(sprite_data->frame_count[h->animation] <= 0){return;}
	entity_mutable->elapsed_time += GetFrameTime();
	if(entity_mutable->elapsed_time >= FLT(SECONDS_PER_FRAME)){
		entity_mutable->elapsed_time = 0.0f;
		int frame_count = sprite_data->frame_count[h->animation];
		int new_frame = entity_mutable->current_frame + 1;
		if(new_frame >= frame_count){
			new_frame = h->play_and_hold ? frame_count - 1 : 0;
		}
		entity_mutable->current_frame = new_frame;
	}	
}
static uint32_t parse_hook_flags(char *val){
	uint32_t out = 0;
	char buf[128];
	snprintf(buf, sizeof(buf), "%s", val);
	char *tok = strtok(buf, "|, ");
	while(tok){
		bool found = false;
		#define X(name) if(check(tok, #name)){out |= (1u << name); found = true;}
		FLAG_LIST	
		#undef X
		if(!found){
			LOG(PARSE, "Unrecognized flag '%s'", tok);
		}
		tok = strtok(NULL, "|, ");
	}
	return out;
}

static void parse_script(struct config_pack p, void *ptr){
	struct Script *s = (struct Script *)ptr;

	enum HookType type = 0;
	bool found = false;
	#define X(name) if(check(p.current_section, #name)){type = name; found = true;}
	HOOKS_LIST
	#undef X
	if(!found){
		LOG(PARSE, "Unrecognized hook section '%s'", p.current_section);
		return;
	}

	if(!s->hooks[type]){
		s->hooks[type] = XCALLOC(1, sizeof(struct Hook));
		s->hooks_bitmask |= (1u << type);
	}
	struct Hook *h = s->hooks[type];

	if(check(p.key, "animation")){t_atoi(p.value, &h->animation); return;}
	if(check(p.key, "play_and_hold")){
		int tmp = 0;
		t_atoi(p.value, &tmp);
		h->play_and_hold = (tmp != 0);
		return;
	}
	if(check(p.key, "flag_add")){h->flag_add = parse_hook_flags(p.value); return;}
	if(check(p.key, "flag_remove")){h->flag_remove = parse_hook_flags(p.value); return;}
}

struct Script *load_script_from_disk(char *path){
	if(!path){
		LOG(IS_NULL, "Null path passed to load_script_from_disk");
		return NULL;
	}
	struct Script *s = XCALLOC(1, sizeof(struct Script));
	if(!config(s, path, parse_script)){
		LOG(NO_FILE, "Failed to load script config at %s", path);
	}
	return s;
}

void free_script(struct Script **s){
	if(!s || !(*s)){return;}
	for(int i=0; i<HOOK_COUNT; i++){
		if((*s)->hooks[i]){
			free((*s)->hooks[i]);
			(*s)->hooks[i] = NULL;
		}
	}
	free(*s);
	*s = NULL;
}
