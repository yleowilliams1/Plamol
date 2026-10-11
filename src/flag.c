#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"
#include "flag.h"
#define GROW_SIZE 5
// eventually this needs to become a real hash table like string table but forn ow this is fine
static uint32_t flag_hash(const char *name){
	// DON'T PASS NULL POINTERS. 
	uint32_t hash = 2166136261u;
	while(*name){
		hash ^= (uint8_t)*name++;
		hash *= 16777619u;
	}
	return hash;
}
static struct Flag *flag_find(struct FlagManager *fm, const char *name, uint32_t hash){
	for(uint32_t i = 0; i < fm->count; i++){
		if(fm->flags[i].hash == hash && strcmp(fm->flags[i].name, name) == 0){
			return &fm->flags[i];
		}
	}
	return NULL;
}
void flag_free(struct FlagManager *fm){
	if(!fm){return;}
	free(fm->flags);
	free(fm);
}
void flag_set(struct FlagManager *fm, const char *name, bool value){
	if(!name || !fm){return;}
	if(strlen(name) >= MAX_FLAG_NAME){return;}  

	uint32_t hash = flag_hash(name);
	struct Flag *f = flag_find(fm, name, hash);
	if(f){
		f->value = value;
		return;
	}

	if(fm->count >= fm->capacity){
		uint32_t new_cap = fm->capacity + GROW_SIZE;
		struct Flag *flags = realloc(fm->flags, sizeof(struct Flag) * new_cap);
		if(!flags){return;}
		fm->flags = flags;
		fm->capacity = new_cap;
	}

	f = &fm->flags[fm->count++];
	f->hash = hash;
	f->value = value;
	strcpy(f->name, name);  
}

bool flag_get(struct FlagManager *fm, const char *name){
	if(!name){LOG(IS_NULL, "name is null"); return false;}
	if(!fm){LOG(IS_NULL, "fm is null"); return false;}

	struct Flag *f = flag_find(fm, name, flag_hash(name));
	return f ? f->value : false;
}
void flip_flag(struct FlagManager *fm){
	if(!fm){LOG(IS_NULL, "fm is null"); return ;}
	fm->count = swap32((uint32_t)fm->count);
	fm->capacity = swap32((uint32_t)fm->capacity);
	for(int i = 0; i < fm->capacity; i++){
		fm->flags[i].hash = swap32((uint32_t)fm->flags[i].hash);	
	}	
}
