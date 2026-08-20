#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <raylib.h>
#include "dou_sprite_manager.h"

#include "t_log_handler.h"
#include "t_config_tool.h"
#include "t_strings.h"

#include "c_dou_struct_defs.h"
#include "c_magic_number.h"

#include "e_dou_manager.h"

static void sprite_on_init(void *slot);
static void sprite_on_pload(void *slot);
static void sprite_on_free(void *slot);
static void sprite_on_load(struct config_pack p, void *ptr);

struct DouLoader dou_sprite(){
	return (struct DouLoader){
		.func.on_load = sprite_on_load,
		.func.on_init = sprite_on_init,
		.func.on_free = sprite_on_free,
		.func.on_pload = sprite_on_pload,
		.size = sizeof(struct DouSpritePrototype),
		.type = EOU_SPRITE,
	};
}

static void sprite_on_init(void *slot){
	// this is super important
	if(!slot){LOG(LOG_NULL, "Can't Post Load sprite since slot is NULL"); return;}
	struct DouSpritePrototype *spr = (struct DouSpritePrototype *)slot;
	spr->animation_count = ANIMATION_COUNT_INVALID;
	// The parser will manually check if 
	// animation_count has parsed by checking against
	// this constant. If it finds that it's invalid at the frame_count phase 
	// then it will abort the program and spit out a
	// error about the ini order	
	LOG(LOG_LOAD, "Initalizing sprite at address %p", slot);
}
static void sprite_on_pload(void *slot){
	if(!slot){LOG(LOG_NULL, "Can't Post Load sprite since slot is NULL"); return;}
	struct DouSpritePrototype *spr = (struct DouSpritePrototype *)slot;
	if(!spr->sprite_path){LOG(LOG_NULL, "Can't load texture since path is NULL"); return;}
	if(spr->texture.id <= 0){
		// texture has not been freed
		UnloadTexture(spr->texture);
	}
	spr->texture = LoadTexture(spr->sprite_path);
	LOG(LOG_LOAD, "Post Loading sprite at address", slot);
}
static void sprite_on_free(void *slot){
	if(!slot){LOG(LOG_NULL, "Can't free null slot"); return;}	
	struct DouSpritePrototype *spr = (struct DouSpritePrototype*)slot;
	if(spr->texture.id > 0){UnloadTexture(spr->texture);}
	
	if(spr->origin){
		for(int i = 0; i < spr->animation_count; i++){
			free(spr->origin[i]);
			spr->origin[i] = NULL;
		}
		free(spr->origin);
		spr->origin = NULL;
	}	
	
	if(spr->frame_count){free(spr->frame_count); spr->frame_count = NULL;}
	if(spr->sprite_path){free(spr->sprite_path); spr->sprite_path = NULL;}
	LOG(LOG_FREE, "Freeing sprite at address %p", slot);
}
static void sprite_on_load(struct config_pack p, void *ptr){
	struct DouSpritePrototype *spr = (struct DouSpritePrototype *)ptr;	
	if(!spr){LOG(LOG_NULL, "Passed null pointer to sprite parser"); return;}
	
	if(t_check(p.current_section, "metadata")){
		if(spr->origin){LOG(LOG_ABORT, "Cannot have metadata placed after origin decloration!");return;}
		if(t_check(p.key, "sprite_path")){
			t_cpy(&spr->sprite_path, p.value);
		}
		if(t_check(p.key, "animation_count")){
			if(spr->animation_count != ANIMATION_COUNT_INVALID){return;}
			t_atoi(p.value, &spr->animation_count);
			if(spr->animation_count <= 0){
				LOG(LOG_ABORT, "%d is not a valid animation_count", spr->animation_count);
				return;
			}
		}
	}

	if(t_check(p.current_section, "frame_counts")){
		if(spr->animation_count == ANIMATION_COUNT_INVALID){
			LOG(LOG_ABORT, "Major error found in sprite ini file. animation_count found in metadata section is placed after frame_count section. Please fix order.");
			return;
		}
		if(spr->origin){LOG(LOG_ABORT, "Can't have frame_counts placed after origin decloration! "); return;}	
		if(!spr->frame_count){spr->frame_count = XCALLOC(1, sizeof(int) * spr->animation_count);}
		for(int i = 0; i < spr->animation_count; i++){
			size_t size = 64;
			char buf[size];
			bool success = t_snprintf(buf, size, NULL, "animation[%d].frame_count", i);
			if(!success){LOG(LOG_NULL, "Failed to t_snprintf");return;}
			if(t_check(p.key, buf)){
				t_atoi(p.value ,&spr->frame_count[i]);	
			}			
		}
	}
	if(t_check(p.current_section, "frame_origins")){
		if(spr->animation_count == ANIMATION_COUNT_INVALID){
			LOG(LOG_ABORT, "Major error found in sprite ini file. animation_count found in metadata section is placed after frame_count section. Please fix order.");
			return;
		}
		if(!spr->frame_count){LOG(LOG_ABORT, "Major error found in sprite ini file. frame_count is placed after frame_origins section. Please remeby.");return;}
		if(!spr->origin){
		    	for(int i = 0; i < spr->animation_count; i++){
				if(spr->frame_count[i] <= 0){
			    		LOG(LOG_ABORT, "Frame count %d cannot have %d as a value", i, spr->frame_count[i]);
			    		return; 
				}
		   	}
		    	spr->origin = XCALLOC(1, spr->animation_count * sizeof(v2*));
		    	for(int i = 0; i < spr->animation_count; i++){
				spr->origin[i] = XCALLOC(1, spr->frame_count[i] * sizeof(v2));
		    	}
		}
		for(int anim = 0; anim < spr->animation_count; anim++){
			for(int frame = 0; frame < spr->frame_count[anim]; frame++){
				size_t size = 64;
				char buf_x[size];
				bool success_x = t_snprintf(buf_x, size, NULL, "anim[%d].frame[%d].x", anim, frame);
				if(!success_x){LOG(LOG_NULL, "Failed to t_snprintf");return;}
				if(t_check(p.key, buf_x)){
					t_atoi(p.value ,&spr->origin[anim][frame].x);	
				}		
				char buf_y[size];
				bool success_y = t_snprintf(buf_y, size, NULL, "anim[%d].frame[%d].y", anim, frame);
				if(!success_y){LOG(LOG_NULL, "Failed to t_snprintf");return;}
				if(t_check(p.key, buf_y)){
					t_atoi(p.value ,&spr->origin[anim][frame].y);	
				}		
			}
		}
	}
}
