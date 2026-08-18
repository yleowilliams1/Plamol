#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "t_log_handler.h"
#include "e_engine_settings.h"
#include "si_flags.h"
#include "t_config_tool.h"

static void flag_parser(struct config_pack p, void *ptr);

struct FlagManager *si_init_flag(){
	struct FlagManager *fm = XCALLOC(1, sizeof(struct FlagManager));
	
	char *path = e_si_to_str(SI_FLAGS);
	if(!path){LOG(LOG_NULL, "e_si_to_str return NULL path for enum %s", SI_FLAGS);}
		
	bool configured = t_config(fm, path, flag_parser);		
	if(!configured){LOG(LOG_ABORT, "Flag failed to configure at %s", path);}
	
	LOG(LOG_LOAD, "Loading flags at %s.", path);
	return fm;
}
bool si_free_flag(struct FlagManager *fm){
	if(!fm){LOG(LOG_FREE, "Can't free flags since it's already NULL");return false;}
	
	free(fm);
	fm = NULL;
	LOG(LOG_FREE, "Freeing flags");
	return true;
}

static void flag_parser(struct config_pack p, void *ptr){
	bool bval = (t_check(p.value, "1") || t_check(p.value, "true"));
	struct FlagManager *fm = ptr;
	if(!fm){LOG(LOG_NULL, "passed ptr is NULL"); return;}
	si_flag_set(p.key, bval, fm);
}
static uint32_t flag_hash(const char *name){
	if(!name){LOG(LOG_NULL, "Passed NULL name to hasher. Returning 0"); return 0;}
	uint32_t hash = 2166136261u;
	while(*name){
		hash ^= (uint8_t)*name++;
		hash *= 16777619u;
	}
	return hash;
}

void si_flag_set(const char *name, bool value, struct FlagManager *fm){
	if(!name){LOG(LOG_NULL, "Passed null name"); return;}
	if(!fm){LOG(LOG_NULL, "Can't set flag since flag manager is null");return;}

	uint32_t hash = flag_hash(name);
	for(int i = 0; i < fm->count; i++){
		if(fm->flags[i].hash == hash){
			fm->flags[i].value = value;
			return;
		}
	}

	LOG(LOG_SET, "Succesfully set flag %s to %d", name, value);
	if(fm->count >= MAX_FLAGS){return;}
	fm->flags[fm->count].hash = hash;
	fm->flags[fm->count].value = value;
	fm->count++;
}
bool si_flag_get(const char *name, struct FlagManager *fm){
	if(!name){LOG(LOG_NULL, "Passed NULL name. Returning false;");return false;}
	if(!fm){LOG(LOG_NULL, "Can't get while flag manager is NULL");return false;}
	uint32_t hash = flag_hash(name);
	for(int i = 0; i < fm->count; i++){
		if(fm->flags[i].hash == hash){
			LOG(LOG_GET, "Got flag %s of value %d", name, fm->flags[i].value);
			return fm->flags[i].value;
		}
	}
	return false;	
}
