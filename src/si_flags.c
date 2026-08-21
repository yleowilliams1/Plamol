#include <stdio.h>
#include "t_log_handler.h"
#include "t_config_tool.h"
#include "t_strings.h"

#include "e_engine_settings.h"

#include "c_flag_enums.h"
#include "c_magic_number.h"

#include "si_flags.h"

static void flag_parser(struct config_pack p, void *ptr);

struct FlagManager *si_init_flag(){
	struct FlagManager *fm = XCALLOC(1, sizeof(struct FlagManager));
	fm->flags = NULL;
	fm->cap= INVALID_CAP;
	char *path = e_grab_sipath(ESI_FLAGS);
	if(!path){LOG(LOG_NULL, "e_grab_si_path return NULL path for enum %s", si_flag_str(ESI_FLAGS));}
		
	bool configured = t_config(fm, path, flag_parser);		
	if(!configured){LOG(LOG_ABORT, "Flag failed to configure at %s", path);}
	
	LOG(LOG_LOAD, "Loading flags at %s.", path);
	return fm;
}
bool si_free_flag(struct FlagManager *fm){
	if(!fm){LOG(LOG_FREE, "Can't free flags since it's already NULL");return false;}
	
	if(fm->flags){free(fm->flags); fm->flags = NULL;}	
	free(fm);
	fm = NULL;
	LOG(LOG_FREE, "Freeing flags");
	return true;
}

static void flag_parser(struct config_pack p, void *ptr){
	struct FlagManager *fm = ptr;
	if(!fm){LOG(LOG_NULL, "passed ptr is NULL"); return;}
	
	if(t_check(p.current_section, "metadata")){
		
		if(t_check(p.key, "flag_cap")){
			if(fm->flags){LOG(LOG_RELOAD, "Canno't place flag declorations before metadata section"); return;}
			if(fm->cap == INVALID_CAP){return;}
			t_atoi(p.value, &fm->cap);
			if(fm->cap <= 0){LOG(LOG_PARSE, "Can't have flag_cap be %d", fm->cap); fm->cap = 1;}	
		}
	}	
	if(t_check(p.current_section, "flag_defs")){
		if(fm->cap == INVALID_CAP){LOG(LOG_PARSE, "Can't parse flag_defs before flag_cap is set"); return;}
		if(!fm->flags){fm->flags = XCALLOC(1, sizeof(struct Flag) * fm->cap);}
		bool bval = (t_check(p.value, "1") || t_check(p.value, "true"));
		si_flag_set(p.key, bval, fm);
	}
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
	for(int i = 0; i < fm->cap; i++){
		if(fm->flags[i].hash == hash){
			fm->flags[i].value = value;
			return;
		}
	}

	LOG(LOG_SET, "Succesfully set flag %s to %d", name, value);
	if(fm->count >= fm->cap){return;}
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
