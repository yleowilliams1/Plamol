#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "e_engine_settings.h"
#include "f_flags.h"

struct FlagManager *fm = NULL;

void f_init_flag(){
	if(fm){return;}
	char *path = e_get_flag_path();
	fm = calloc(1, sizeof(struct FlagManager));	
	
	char line[256];
	char current_section[64] = {0};
	FILE *f = fopen(path, "r");
	if(!f){return;}
	while(fgets(line, sizeof(line), f)){
		if(line[0] == '\n' || line[0] == '#' || line[0] == ';'){continue;}
		if(line[0] == '['){
			sscanf(line, "[%63[^]]]", current_section);
			continue;
		}
		char key[64];
		char value[128];

		if(sscanf(line, "%63[^=]=%127[^\n]", key, value) == 2){
			bool bval = (strcmp(value, "1") == 0 || strcmp(value, "true") == 0); 
			flag_set(key, bval);		
		}
	}	
}
void f_free_flag(){
	if(!fm){return;}
	free(fm);
}
uint32_t flag_hash(const char *name){
	uint32_t hash = 2166136261u;
	while(*name){
		hash ^= (uint8_t)*name++;
		hash *= 16777619u;
	}
	return hash;
}

void flag_set(const char *name, bool value){
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
	uint32_t hash = flag_hash(name);
	for(int i = 0; i < fm->count; i++){
		if(fm->flags[i].hash == hash){
			return fm->flags[i].value;
		}
	}
	return false;	
}
