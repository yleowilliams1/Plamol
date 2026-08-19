#include "c_types.h"
#include "t_log_handler.h"
#include "dou_sprite_manager.h"
#include "inst_instances.h"
#include "e_dou_manager.h"
#include "e_engine_settings.h"

// This is pretty bad since e_dou_get calls t_gindx_to_lindx and this is ran multiple times a frame.
// t_gindx_to_lindx does a for loop so this is really not great but i'll fix later since it's going to
// need a rework
void inst_anim_advance(struct InstanceChildAnimState *anim, struct DouManager *dou, int gindx, float delta){
	if(!anim)LOG(LOG_NULL, "Anim is NULL on gindx %d", gindx);{return;}
	if(!dou){LOG(LOG_NULL, "Dou is NULL on gindx %d", gindx);return;}
	
	struct SpriteData *spr = e_dou_get(dou, gindx, DOU_SPRITE);
	if(!spr){LOG(LOG_NULL, "Sprite return NULL on gindx %d", gindx);return;}	
	
	anim->elapsed_time += delta;
	if(anim->elapsed_time >= e_grab_animfps()){
				
	}	
	
}
int inst_derive_state(struct StatChildInstance *stat, enum Dev type){

}
