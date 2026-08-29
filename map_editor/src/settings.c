#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "settings.h"
#include "log.h"
#include "string.h"
#include "config.h"

#define INI_PATH "data/config/settings.ini"

static void parse_settings(struct config_pack p, void *ptr);
static enum WindowOptions window_option_from_str(const char *str);
static void apply_window_option(enum WindowOptions opt);
static bool is_bit_set(int n);

static struct Settings *settings = NULL;

void load_settings(char *path){
	if(settings){free_settings();}
	if(!path){LOG(LOG_NULL, "Can't load settings, passed NULL path");return;}
	settings = XCALLOC(1, sizeof(struct Settings));
	config(settings, path, parse_settings);	
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
}
void apply_settings(){
	apply_window_option(settings->window_option);
}
char *STR(enum SettingsStrings str){if(!settings){return NULL;}return settings->strings[str];}
int INT(enum SettingsIntegers in){if(!settings){return -1;}return settings->integers[in];}
float FLT(enum SettingsFloats flt){if(!settings){return -1;}return settings->floats[flt];}
bool FLG(enum SettingsFlags flg) {if(!settings){return false;}return is_bit_set(flg);}

static void parse_settings(struct config_pack p, void *ptr){
	if(!ptr){LOG(LOG_NULL, "Passed a NULL pointer to the engine parser."); return;}
	if(check(p.key, "WindowOptions")){
		enum WindowOptions op = window_option_from_str(p.value);
		if(op != WINDOW_OPTION_COUNT){settings->window_option = op;}
	}	

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
static bool is_bit_set(int n) {
    return (settings->flags & (1 << n)) != 0;
}
static enum WindowOptions window_option_from_str(const char *str){
	#define X(id, fnc) if(strcmp(str, #id) == 0) return id;
	WINDOW_OPTIONS
	#undef X
	return WINDOW_OPTION_COUNT;
}
static void apply_window_option(enum WindowOptions opt){
	switch(opt){
		#define X(id, fnc) case id: fnc(); break;
		WINDOW_OPTIONS
		#undef X
		case WINDOW_OPTION_COUNT: break;
	}
}
