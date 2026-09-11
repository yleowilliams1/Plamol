#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "settings.h"
#include "util/util.h"

static void parse_settings(struct config_pack p, void *ptr);

static struct Settings *settings = NULL;

void load_settings(char *path){
	if(settings){free_settings();}
	if(!path){LOG(IS_NULL, "Can't load settings, passed NULL path");return;}
	settings = XCALLOC(1, sizeof(struct Settings));
	for(int i = 0; i < SETTINGS_INTEGER_COUNT; i++) {
		settings->integers[i] = -1; 
	}
	for(int i = 0; i < SETTINGS_FLOAT_COUNT; i++) {
		settings->floats[i] = -1.0f; 
	}
	config(settings, path, parse_settings);	
	for(int i = 0; i < SETTINGS_STRINGS_COUNT; i++){
		if(settings->strings[i] != NULL){
			LOG(LOAD, "Loaded %s as %s", STR(i), settings->strings[i]);
			continue;
		}
		LOG(IS_NULL, "String %s has not been set!", settings_string_str(i));
	}
	for(int i = 0; i < SETTINGS_INTEGER_COUNT; i++){
		if(settings->integers[i] != -1){
			LOG(LOAD, "Loaded %s as %d", INT(i), settings->integers[i]);
			continue;
		}
		LOG(IS_NULL, "Integer %s has not been set!", settings_integer_str(i));
	}
	for(int i = 0; i < SETTINGS_FLOAT_COUNT; i++){
		if(settings->floats [i] != -1.0f){
			LOG(LOAD, "Loaded %s as %f", FLT(i), settings->floats[i]);
			continue;
		}
		LOG(IS_NULL, "Float %s has not been set!", settings_float_str(i));
	}
	for(int i = 0; i < SETTINGS_FLAG_COUNT; i++){
		if(is_bit(settings->flags, i)){
			LOG(LOAD, "Set %s to true", FLG(i));
			continue;
		}
	}
	
	LOG(LOAD, "Loading settings");
}
void free_settings(){
	if(!settings){return;}
	for(int i = 0; i < SETTINGS_STRINGS_COUNT; i++){
		if(!settings->strings[i]){continue;}
		free(settings->strings[i]);
		settings->strings[i] = NULL;
	}	
	free(settings);
	settings = NULL;
	LOG(FREE, "Freed settings");
}
char *STR(enum SettingsStrings str){if(!settings){LOG(IS_NULL, "Settings is NULL");return NULL;}return settings->strings[str];}
int INT(enum SettingsIntegers in){if(!settings){LOG(IS_NULL, "Settings is NULL");return -1;}return settings->integers[in];}
float FLT(enum SettingsFloats flt){if(!settings){LOG(IS_NULL, "Settings is NULL");return -1;}return settings->floats[flt];}
bool FLG(enum SettingsFlags flg) {if(!settings){LOG(IS_NULL, "Settings is NULL");return false;}return is_bit(settings->flags ,flg);}

static void parse_settings(struct config_pack p, void *ptr){
	if(!ptr){LOG(IS_NULL, "Passed a NULL pointer to the engine parser."); return;}

	for(int i = 0; i < SETTINGS_STRINGS_COUNT; i++){
		if(!check(p.key, (char *)settings_string_str(i))){continue;};		
		t_cpy(p.value, &settings->strings[i]);
	}
	for(int i = 0; i < SETTINGS_INTEGER_COUNT; i++){
		if(!check(p.key, (char *)settings_integer_str(i))){continue;};		
		t_atoi(p.value, &settings->integers[i]);
	}
	for(int i = 0; i < SETTINGS_FLOAT_COUNT; i++){
		if(!check(p.key, (char *)settings_float_str(i))){continue;};		
		t_atof(p.value, &settings->floats[i]);
	}
	for(int i = 0; i < SETTINGS_FLAG_COUNT; i++){
		if(!check(p.key, (char *)settings_flag_str(i))){continue;};		
		int value; t_atoi(p.value, &value);
		if(value > 0){settings->flags |= (1 << i);}
	}
}
