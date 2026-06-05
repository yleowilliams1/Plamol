#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e_engine_settings.h"
#include "i_input.h"

#define INI_PATH "data/engine.ini"
#define MAX_BINDS 64
static struct EngineSettings *settings;

char *e_get_map_path(){
	return settings->map_path;
}
char *e_get_path_to_tilesets(){
	return settings->path_to_tilesets;
}

static bool parse_bind(const char *action_str, const char *value_str, struct SettingsBind *out){
	/* Resolve action name */
	int action = -1;
	if      (strcmp(action_str, "A_WALK_LEFT")  == 0) action = A_WALK_LEFT;
	else if (strcmp(action_str, "A_WALK_RIGHT") == 0) action = A_WALK_RIGHT;
	else if (strcmp(action_str, "A_WALK_UP")    == 0) action = A_WALK_UP;
	else if (strcmp(action_str, "A_WALK_DOWN")  == 0) action = A_WALK_DOWN;
	else if (strcmp(action_str, "A_ACTION")     == 0) action = A_ACTION;
	else { return false; }

	/* Resolve type name and numeric key: "KEYBOARD,65" */
	char type_str[32];
	int  key_code;
	if (sscanf(value_str, "%31[^,],%d", type_str, &key_code) != 2) { return false; }

	int type = -1;
	if      (strcmp(type_str, "KEYBOARD") == 0) type = KEYBOARD;
	else if (strcmp(type_str, "MOUSE")    == 0) type = MOUSE;
	else if (strcmp(type_str, "GAMEPAD")  == 0) type = GAMEPAD;
	else { return false; }

	out->action = action;
	out->type   = type;
	out->key    = key_code;
	return true;
}

void e_free_setting(){
	free(settings->default_binds_list);
	free(settings->override_binds_list);
	free(settings->map_path);
	free(settings->path_to_tilesets);
	free(settings);
}

bool e_load_engine_settings(){
	char line[256];
	char current_section[64] = {0};
	
	FILE *f = fopen(INI_PATH, "r");
	if(!f) {return false;}
	
	settings = malloc(sizeof(struct EngineSettings));
	settings->default_binds_list  = calloc(MAX_BINDS, sizeof(struct SettingsBind));
	settings->override_binds_list = calloc(MAX_BINDS, sizeof(struct SettingsBind));
	settings->default_binds_count  = 0;
	settings->override_binds_count = 0;
			
	while(fgets(line, sizeof(line), f)){
		if (line[0] == '\n' || line[0] == '#' || line[0] == ';'){continue;}
		if (line[0] == '[') {
			sscanf(line, "[%63[^]]]", current_section);
			continue;
	    	}

	    	char key[64], value[128];
	    	if (sscanf(line, "%63[^=]=%127[^\n]", key, value) == 2) {
			/* trim leading whitespace from value */
			char *v = value;
			while (*v == ' ' || *v == '\t') v++;
			
			if(strcmp(current_section, "general") == 0){
				if(strcmp(key, "map_path") == 0){
					settings->map_path = strdup(v);
				}
				if(strcmp(key, "path_to_tilesets") == 0){
					settings->path_to_tilesets = strdup(v);
				}
			} else if(strcmp(current_section, "default_binds") == 0){
				if(settings->default_binds_count < MAX_BINDS){
					struct SettingsBind *slot = &settings->default_binds_list[settings->default_binds_count];
					if(parse_bind(key, v, slot)){
						settings->default_binds_count++;
					}
				}
			} else if(strcmp(current_section, "override_binds") == 0){
				if(settings->override_binds_count < MAX_BINDS){
					struct SettingsBind *slot = &settings->override_binds_list[settings->override_binds_count];
					if(parse_bind(key, v, slot)){
						settings->override_binds_count++;
					}
				}
			}
	    	}	
	}
	for(int i = 0; i < settings->default_binds_count; i++){
		struct SettingsBind *b = &settings->default_binds_list[i];
		i_set_binding(b->type, b->action, b->key);
	}
	for(int i = 0; i < settings->override_binds_count; i++){
		struct SettingsBind *b = &settings->override_binds_list[i];
		i_set_binding(b->type, b->action, b->key);
	}

	fclose(f);
	return true;

