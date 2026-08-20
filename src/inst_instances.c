#include <math.h>
#include "t_log_handler.h"
#include "dou_sprite_manager.h"

#include "inst_instances.h"

#include "e_dou_manager.h"
#include "e_engine_settings.h"

// This is pretty bad since e_dou_get calls t_gindx_to_lindx and this is ran multiple times a frame.
// t_gindx_to_lindx does a for loop so this is really not great but i'll fix later since it's going to
// need a rework
void inst_anim_advance(struct InstanceChildAnimState *anim, struct DouManager *dou, int gindx, float delta){
	if(!anim){LOG(LOG_NULL, "Anim is NULL on gindx %d", gindx);return;}
	if(!dou){LOG(LOG_NULL, "Dou is NULL on gindx %d", gindx);return;}
	
	struct SpriteData *spr = e_dou_get(dou, gindx, DOU_SPRITE);
	if(!spr){LOG(LOG_NULL, "Sprite return NULL on gindx %d", gindx);return;}	
	
	anim->elapsed_time += delta;
	if(anim->elapsed_time >= e_grab_animfps()){
		anim->current_frame += 1;
		if(anim->current_frame >= spr->frame_count[anim->current_animation]){
			anim->current_frame = 0;	
		}
	}	
	
}
int inst_derive_stat(struct StatChildInstance *stat, enum Dev type){
	if(!stat){LOG(LOG_NULL, "Stat is NULL");return 0;}
	if(type < 0 || type > DERV_CAP){LOG(LOG_NULL, "%d is not a valid type", type);return 0;}
	int *b = stat->base; 
	switch(type){
		case PHYS_CORD:
			return (int)ceil(((b[STR] + b[DEX] - b[INT]) / 2.0) / 4.0);
			break;
		case WORD:
			return (int)ceil(((b[SOC] + b[INT] - b[CON]) / 2.0) / 4.0);
			break;
		case PROB_ANALYSIS:
			return (int)ceil(((b[INT] + b[WIS] - b[STR]) / 2.0) / 4.0);
			break;
		case SPATIAL:
			return (int)ceil(((b[INT] + b[DEX] - b[SOC]) / 2.0) / 4.0);
			break;
		case MUSICAL:
			return (int)ceil(((b[WIS] + b[DEX] - b[SOC]) / 2.0) / 4.0);
			break;
		case NATURAL:
			return (int)ceil(((b[WIS] + b[CON] - b[DEX]) / 2.0) / 4.0);
			break;
		case INTERPERSONAL:
			return (int)ceil(((b[SOC] + b[WIS] - b[STR]) / 2.0) / 4.0);
			break;
		case INTRAPERSONAL:
			return (int)ceil(((b[INT] + b[CON] - b[WIS]) / 2.0) / 4.0);
			break;
		case INNOCENCE:
			return (int)ceil(((b[CON] + b[DEX] - b[INT]) / 2.0) / 4.0);
			break;
		case HEROISM:
			return (int)ceil(((b[STR] + b[SOC] - b[INT]) / 2.0) / 4.0);
			break;
		case LOVE:
			return (int)ceil(((b[SOC] + b[CON] - b[DEX]) / 2.0) / 4.0);
			break;
		case AUTHORITARIAN:
			return (int)ceil(((b[STR] + b[INT] - b[SOC]) / 2.0) / 4.0);
			break;
		case AC:
			return (int)ceil((b[DEX] + b[WIS]) - b[STR]);
			break;
		case DR:
			return (int)ceil((b[STR] + b[CON]) - b[DEX]);
			break;
		case MAX_HP:
			return (int)ceil((b[CON] + b[STR]) - b[DEX]);
			break;
		case INITATIVE:
			return (int)ceil(((b[DEX] + b[WIS]) - b[INT]) / 4.0);
			break;
		default:
			LOG(LOG_OUTOFBOUNDS, "%d wasn't caught by switch statement", type);
			return 0;
			break;
	}
	return true;
}
