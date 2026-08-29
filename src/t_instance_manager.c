#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "t_instance_manager.h"
#include "t_depot_manager.h"
#include "t_math.h"
#include "t_log_handler.h"

#include "depo_sprite.h"

#include "e_engine_settings.h"

#include "si_map.h"

#define VERSION 1
#define MAGIC_NUMBER 0x42656E736F6E415A
#define EXPECTED_ENDIAN 0xFFFE
#define SWAPPED_ENDIAN 0xFEFF

static bool si_valid_header(struct InstanceHeader *header);

struct InstanceManager *t_create_instance_manager(struct InstanceFunctions fncs, int count, size_t size){
	int instance_count = count;
	struct InstanceManager *iman = XCALLOC(1, sizeof(struct InstanceManager));	
	if(count > 1){iman->instances = XCALLOC(1, sizeof(struct Instance *) * instance_count);	}
	iman->fncs = fncs;
	iman->count = count;	
	iman->prototype_size = size;
	
	return iman;
}
void t_free_instance_manager(struct InstanceManager *instance_manager){
	if(!instance_manager){return;}
	
	if(instance_manager->instances){
		for(int i = 0; i < instance_manager->count; i++){
			if(!instance_manager->instances[i]){continue;}
			// lmao leave me alone it's fine if i say it's a one liner it's a one liner
			if(instance_manager->instances[i]->prototype_copy){if(instance_manager->fncs.on_free){instance_manager->fncs.on_free(instance_manager->instances[i]->prototype_copy);} else {free(instance_manager->instances[i]->prototype_copy);}}
			instance_manager->instances[i]->prototype_copy = NULL;
			free(instance_manager->instances[i]);
			instance_manager->instances[i] = NULL;
		}
	}
	free(instance_manager);
	instance_manager = NULL;
}
void t_save_instance_manager(struct InstanceManager *iman, char *path){
	if(!iman || !path || !iman->instances){LOG(LOG_NULL, "Is null");return;}	
	
	FILE *f = fopen(path, "wb");
	if(!f){return;}
	struct InstanceHeader header = {0};

	header.time_stamp = (uint32_t)time(NULL);
	header.magic_number = MAGIC_NUMBER;
	header.endian_check = 0xFFFE;
	header.version = VERSION;

	fwrite(&header, sizeof(struct InstanceHeader), 1, f);
	fwrite(&iman->count, sizeof(int), 1, f);
	for(int i = 0; i < iman->count; i++){
		struct Instance *inst = iman->instances[i];
		if(!inst){continue;}

		fwrite(&inst->is_global_coordinates, sizeof(bool), 1, f);
		fwrite(&inst->can_passthrough, sizeof(bool), 1, f);
		fwrite(&inst->pos, sizeof(v3), 1, f);
		fwrite(&inst->anim, sizeof(struct AnimationState), 1, f);
		fwrite(&inst->facing, sizeof(enum Direction), 1, f);

		if(!iman->fncs.on_serialize){LOG(LOG_NULL, "Tried to serialize with NULL serialize pointer!!!!"); continue;}
		iman->fncs.on_serialize(f, inst->prototype_copy);
	}		

	fclose(f);
}
void t_load_instance_manager(struct InstanceManager *iman, char *path){
        if(!iman || !path || !iman->instances){LOG(LOG_NULL, "Is null");return;}
        FILE *f = fopen(path, "rb");
        if(!f){return;}

        struct InstanceHeader header = {0};
        fread(&header, sizeof(struct InstanceHeader), 1, f);
        if(!si_valid_header(&header)){fclose(f); return;}

        int count = 0;
        if(fread(&count, sizeof(int), 1, f) != 1){
                LOG(LOG_NULL, "Failed to read instance count");
                fclose(f);
                return;
        }

        struct Instance **new_instances = malloc(sizeof(struct Instance*) * count);
        if(!new_instances){
                LOG(LOG_NULL, "Failed to allocate instances array");
                fclose(f);
                return;
        }

        for(int i = 0; i < count; i++){
                struct Instance *inst = malloc(sizeof(struct Instance));
                if(!inst){
                        LOG(LOG_NULL, "Failed to allocate instance");
                        count = i;
                        goto fail;
                }

                fread(&inst->is_global_coordinates, sizeof(bool), 1, f);
                fread(&inst->can_passthrough, sizeof(bool), 1, f);
                fread(&inst->pos, sizeof(v3), 1, f);
                fread(&inst->anim, sizeof(struct AnimationState), 1, f);
                fread(&inst->facing, sizeof(enum Direction), 1, f);

                inst->prototype_copy = malloc(iman->prototype_size);
                if(!inst->prototype_copy){
                        LOG(LOG_NULL, "Failed to allocate prototype_copy");
                        free(inst);
                        count = i;
                        goto fail;
                }

                if(!iman->fncs.on_deserialize){
                        LOG(LOG_NULL, "Tried to deserialize with NULL deserialize pointer!!!!");
                } else {
                        iman->fncs.on_deserialize(inst->prototype_copy, f);
                }

                new_instances[i] = inst;
        }

        if(iman->instances){
                for(int i = 0; i < iman->count; i++){
                        if(iman->instances[i]){
                                free(iman->instances[i]->prototype_copy);
                                free(iman->instances[i]);
                        }
                }
                free(iman->instances);
        }

        iman->instances = new_instances;
        iman->count = count;

        fclose(f);
        return;

fail:
        for(int i = 0; i < count; i++){
                free(new_instances[i]->prototype_copy);
                free(new_instances[i]);
        }
        free(new_instances);
        fclose(f);
}
void t_populate_instance_manager(struct InstanceManager *iman, struct DepotManager *dman, int depot_index, struct InstanceSlot *islots, int islot_count, char *path){
	if(!iman){return;}// Theres no log because i'll do a full log assert pass later so this is temporary
	if(!dman){return;}
	if(!islots){return;}
	
	if(path){
		t_load_instance_manager(iman, path);	
		return;
	}
	for(int i = 0; i < islot_count; i++){
		struct InstanceSlot *src = &islots[i];
		if(src->instance_gindx < 0 || src->instance_gindx >= iman->count){continue;}
		struct Instance *dest = iman->instances[src->instance_gindx];
		if(dest){LOG(LOG_RELOAD, "Tried to reload a dest of instance gindx %d which is already allocated!!!!!!", src->instance_gindx);return;}
		dest = XCALLOC(1, sizeof(struct Instance));
		iman->instances[src->instance_gindx] = dest;
		dest->is_global_coordinates = src->is_global_coordinates;
		dest->can_passthrough = src->can_passthrough;
		dest->pos = src->position;
		dest->facing = src->direction;
		dest->prototype_copy = XCALLOC(1, iman->prototype_size);
	memcpy(dest->prototype_copy, t_grab_item(dman, depot_index, src->prototype_gindx, iman->prototype_size), iman->prototype_size);
		t_free_item(dman, depot_index, src->prototype_gindx);	
		// We don't actualyl need to prototype loaded anymore so it's fine to free.	
		// Now we have the default stuff, we can override changes with the save file
	}
}


static bool si_valid_header(struct InstanceHeader *header){
	if(!header){LOG(LOG_NULL, "header was NULL");return false;}
	if(header->magic_number != MAGIC_NUMBER){LOG(LOG_PARSE, "Magic number does not match"); return false;}
	if(header->endian_check == SWAPPED_ENDIAN){LOG(LOG_PARSE, "Endian mismatch ,need to byteswap");return false;}
	if(header->endian_check != SWAPPED_ENDIAN && header->endian_check != EXPECTED_ENDIAN){LOG(LOG_NULL, "Header is corrupted");return false;}
	if(header->version != VERSION){LOG(LOG_OUTOFBOUNDS, "Possibly map won't be read since they are on differing version from compiled executable");}
	return true;
}
