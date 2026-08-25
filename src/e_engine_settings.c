#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_engine_settings.h"

#include "t_config_tool.h"
#include "t_log_handler.h"
#include "t_strings.h"

#include "c_depot_list.h"

#define INI_PATH "data/engine.ini"

static void engine_parser(struct config_pack p, void *ptr);

static struct EngineSettings *settings;

void e_free_settings(){
	if(!settings){LOG(LOG_FREE, "Can't free settings since it's already NULL");return;}
	for(int i = 0; i < SI_COUNT; i++){
		if(!settings->si_paths[i]){continue;}
		free(settings->si_paths[i]);
		settings->si_paths[i] = NULL;
	}
	for(int i = 0; i < DEPOT_COUNT; i++){
		if(settings->depo_paths[i]){free(settings->depo_paths[i]);}
		if(settings->depo_formats[i]){free(settings->depo_formats[i]);}
		
		settings->depo_item_counts[i] = 0;
		settings->depo_paths[i] = NULL;
		settings->depo_formats[i] = NULL;
	}
	free(settings);
	settings = NULL;
	LOG(LOG_FREE, "Freed settings");
}
void e_load_engine_settings(){
	if(settings){
		e_free_settings();
	}

	settings = XCALLOC(1, sizeof(struct EngineSettings));
	bool configured = t_config(settings, INI_PATH, engine_parser);
	if(!configured){LOG(LOG_ABORT, "t_config return false. Configuration failed"); return;}
	LOG(LOG_LOAD, "Loading engine settings at relative path from executable %s", INI_PATH);
}

char *e_grab_sipath(enum SiEnum si){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return NULL;}
	return settings->si_paths[si];
}
float e_grab_animfps(){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return 12;}
	return settings->anim_fps;
}

char *e_grab_depopath(enum DepotType depot_index){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return NULL;}
	return settings->depo_paths[depot_index];
}
char *e_grab_depoformat(enum DepotType depot_index){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return NULL;}
	return settings->depo_formats[depot_index];
}
int e_grab_depoitemcount(enum DepotType depot_index){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return 1;}
	return settings->depo_item_counts[depot_index];
}
static void engine_parser(struct config_pack p, void *ptr){
	struct EngineSettings *s = ptr;
	if(!s){LOG(LOG_NULL, "NULL settings in parser");return;}
	if(t_check(p.current_section, "Paths")){
		for(int i = 0; i < SI_COUNT; i++){
			if(!t_check(p.key, (char *)sistr(i))){continue;}
			t_cpy(&s->si_paths[i], p.value);
		}
		for(int i = 0; i < DEPOT_COUNT; i++){
			if(!t_check(p.key, (char *)depstr(i))){continue;}
			t_cpy(&s->depo_paths[i], p.value);
		}
	}
	if(t_check(p.current_section, "Memory")){
		for(int i = 0; i < DEPOT_COUNT; i++){
			size_t size = 128;
			char buf[size];
			char *base = (char *)depstr(i); 
			t_snprintf(buf, size, NULL, "%s%s", base, "Count");	
			if(!t_check(p.key, buf)){continue;}
			t_atoi(p.value, &s->depo_item_counts[i]);
		}
	}
	if(t_check(p.current_section, "Format")){
		for(int i = 0; i < DEPOT_COUNT; i++){
			size_t size = 128;
			char buf[size];
			char *base = (char *)depstr(i); 
			t_snprintf(buf, size, NULL, "%s%s", base, "Format");	
			if(!t_check(p.key, buf)){continue;}
			t_cpy(&s->depo_formats[i], p.value);
		}
	}
	if(t_check(p.current_section, "Animation")){
		if(t_check(p.key, "AnimationFps")){
			t_atof(p.value, &s->anim_fps);
		}
	}
}
