#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "e_error_handler.h"
#include "e_engine_settings.h"
#include "f_flags.h"
#include "t_config_tool.h"
struct FlagManager *fm = NULL;
static void flag_parser(struct config_pack p, void *ptr){
	// Overkill but who fucking cares.
	bool bval = (t_check(p.value, "1") || t_check(p.value, "true"));
	flag_set(p.key, bval);
}
void f_init_flag(){
	if(fm){
		ERR_LOG(ERR_RELOAD, "Attempted to initalize flag without freeing.");
		f_free_flag();
	}
	char *path = e_grab_str(FLAG_PATH);
	// The only possible way that path is NULL is if 
	// FLAG_PATH is larger than ENG_STR_COUNT, or if it's less than 0.
	// Neither of which is possible.
	// All other NULLs result in a crash.
	
	fm = XCALLOC(1, sizeof(struct FlagManager));	
	// Basically same thing here.
	// Super unlikely this will return NULL	
		
	t_config(NULL, path, flag_parser);		
	// t_config will crash if it fails so don't check.
}
void f_free_flag(){
	if(!fm){
		ERR_LOG(ERR_RELOAD, "Attempted to double free flag manager");
		return;
	}
	free(fm);
	fm = NULL;
}
static int32_t flag_hash(const char *name){
	// DON'T PASS NULL POINTERS. 
	uint32_t hash = 2166136261u;
	while(*name){
		hash ^= (uint8_t)*name++;
		hash *= 16777619u;
	}
	return hash;
}

void flag_set(const char *name, bool value){
	// Flag set exclusivly gets called by the 
	// init parser. So it's a parser kill
	if(!name){
		ERR_LOG(ERR_FUCKED, "Passed NULL name to flag setter during parsing. This really really really shouldn't happen. ");
	}
	if(!fm){
		ERR_LOG(ERR_FUCKED, "Tried to call before flag was loaded. BIG FUCKIN NONO. This really shouldn't have happened.");
	}	
	uint32_t hash = flag_hash(name);
	for(int i = 0; i < fm->count; i++){
		if(fm->flags[i].hash == hash){
			fm->flags[i].value = value;
			return;
		}
	}
	if(fm->count >= MAX_FLAGS){return;}
	fm->flags[fm->count].hash = hash;
	fm->flags[fm->count].value = value;
	fm->count++;
}
bool flag_get(const char *name){
	if(!name){
		ERR_LOG(ERR_PARSE, "Major issue. Tried to get flag with NULL name. Valve please fix please.");
		return false;
	}
	if(!fm){
		ERR_LOG(ERR_FUCKED, "Big nono. Can't call flag get while flag is NULL. Get fucked");
	}
	uint32_t hash = flag_hash(name);
	for(int i = 0; i < fm->count; i++){
		if(fm->flags[i].hash == hash){
			return fm->flags[i].value;
		}
	}
	return false;	
}
