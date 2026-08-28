#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "t_instance_manager.h"
#include "t_depot_manager.h"
#include "t_math.h"
#include "t_log_handler.h"

#include "depo_sprite.h"

#include "e_engine_settings.h"

#include "si_map.h"

struct InstanceManager *t_create_instance_manager(struct InstanceFunctions fncs, int count){
	int instance_count = count;
	struct InstanceManager *iman = XCALLOC(1, sizeof(struct InstanceFunctions));	
	if(count > 1){iman->instances = XCALLOC(1, sizeof(struct Instance *) * instance_count);	}
	iman->fncs = fncs;
	iman->count = count;	
	
	return iman;
}
void t_free_instance_manager(struct InstanceManager *instance_manager){
	if(!instance_manager){return;}
	
	if(instance_manager->instances){
		for(int i = 0; i < instance_manager->count; i++){
			if(!instance_manager->instances[i]){continue;}
			if(instance_manager->instances[i]->prototype_copy){free(instance_manager->instances[i]->prototype_copy);}
			instance_manager->instances[i]->prototype_copy = NULL;
			free(instance_manager->instances[i]);
			instance_manager->instances[i] = NULL;
		}
	}
	free(instance_manager);
	instance_manager = NULL;
}
void t_populate_instance_manager(struct InstanceManager *iman, struct DepotManager *dman, int depot_index, struct InstanceSlot *islots, int islot_count, size_t size){
	if(!iman){return;}// Theres no log because i'll do a full log assert pass later so this is temporary
	if(!dman){return;}
	if(!islots){return;}
	for(int i = 0; i < islot_count; i++){
		struct InstanceSlot *src = &islots[i];
		if(src->instance_gindx < 0 || src->instance_gindx > iman->count){continue;}
		struct Instance *dest = iman->instances[src->instance_gindx];
		if(dest){LOG(LOG_RELOAD, "Tried to reload a dest of instance gindx %d which is already allocated!!!!!!", src->instance_gindx);return;}
		dest = XCALLOC(1, size);
		dest->is_global_coordinates = src->is_global_coordinates;
		dest->can_passthrough = src->can_passthrough;
		dest->pos = src->position;
		dest->facing = src->direction;
		memcpy(&dest->prototype_copy, t_grab_item(dman, depot_index, src->prototype_gindx, size), size);
		t_free_item(dman, depot_index, src->prototype_gindx);	
		// We don't actualyl need to prototype loaded anymore so it's fine to free.	
		// Now we have the default stuff, we can override changes with the save file
	}	
}


