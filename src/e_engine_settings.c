#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e_engine_settings.h"
#include "t_config_tool.h"
#include "t_log_handler.h"
#include "t_strings.h"
#define INI_PATH "data/engine.ini"
static void engine_parser(struct config_pack p, void *ptr);

static struct EngineSettings *settings;

char *e_si_to_str(enum SiEnum type){
	if(type < 0 || type >= SI_COUNT){LOG(LOG_OUTOFBOUNDS, "%s is not a valid singles enum", type);}
	const char *si_strs[SI_COUNT] = {
		[SI_FLAGS] = "Flag",
		[SI_STATE] = "State",
		[SI_INPUT] = "Input",
		[SI_MAP]   = "Map",
		[SI_GAMESTATE] = "Gamestate",
	};
	return (char *)si_strs[type];
};
char *e_in_to_str(enum InstancesEnum type){
	if(type < 0 || type >= INSTANCE_ENUM_COUNT){LOG(LOG_OUTOFBOUNDS, "%s is not a valid singles enum", type);}
	const char *in_strs[INSTANCE_ENUM_COUNT] = {
		[INSTANCE_ENTITY] = "Entity",
		[INSTANCE_INTERACTABLE] = "Interactable",
	};
	return (char *)in_strs[type];
};
void e_free_settings(){
	if(!settings){LOG(LOG_FREE, "Can't free settings since it's already NULL");return;}
	for(int i = 0; i < SI_COUNT; i++){
		if(!settings->si_paths[i]){continue;}
		free(settings->si_paths[i]);
		settings->si_paths[i] = NULL;
	}
	for(int i = 0; i < DOU_COUNT; i++){
		if(settings->dou_paths[i]){free(settings->dou_paths[i]);}
		if(settings->dou_formats[i]){free(settings->dou_formats[i]);}
		settings->dou_icounts[i] = 0;
		settings->dou_paths[i] = NULL;
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
	
	for(int i = 0; i < SI_COUNT; i++){
		if(settings->si_paths[i] == NULL){
			LOG(LOG_NULL, "%s didn't load and is currently NULL", e_si_to_str(i));
		}
	}
	for(int i = 0; i < DOU_COUNT; i++){
		if(settings->dou_paths[i] == NULL){
			LOG(LOG_NULL, "%s format didn't load and is currently NULL", e_dou_to_str(i));
		}
		if(settings->dou_formats[i] == NULL){
			LOG(LOG_NULL, "%s format didn't load and is currently NULL", e_dou_to_str(i));
		}
		if(settings->dou_icounts[i] == 0){
			LOG(LOG_NULL, "%s count is current set to 0", e_dou_to_str(i));
		}
	}
}

char *e_grab_sipath(enum SiEnum si){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return NULL;}
	return settings->si_paths[si];	
}
char *e_grab_doupath(enum DouEnum dou){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return NULL;}
	return settings->dou_paths[dou];

}
char *e_grab_douformat(enum DouEnum dou){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return NULL;}
	return settings->dou_formats[dou];
}
int e_grab_doucount(enum DouEnum dou){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return 0;}
	return settings->dou_icounts[dou];
}
int e_grab_inscount(enum InstancesEnum type){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return 0;}
	return settings->instance_counts[type];
}
float e_grab_animfps(){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return 0;}
	return settings->anim_fps;
}
static void engine_parser(struct config_pack p, void *ptr){
	struct EngineSettings *s = ptr;
	if(!s){LOG(LOG_NULL, "NULL settings in parser");return;}
	if(t_check(p.current_section, "Paths")){
		for(int i = 0; i < SI_COUNT; i++){
			if(!t_check(p.key, e_si_to_str(i))){continue;}
			t_cpy(&s->si_paths[i], p.value);
		}
		for(int i = 0; i < DOU_COUNT; i++){
			if(!t_check(p.key, e_dou_to_str(i))){continue;}
			t_cpy(&s->dou_paths[i], p.value);
		}
	}
	if(t_check(p.current_section, "Memory")){
		for(int i = 0; i < DOU_COUNT; i++){
			size_t size = 128;
			char buf[size];
			char *base = e_dou_to_str(i); 
			t_snprintf(buf, size, NULL, "%s%s", base, "Count");	
			if(!t_check(p.key, buf)){continue;}
			t_atoi(p.value, &s->dou_icounts[i]);
		}
	}
	if(t_check(p.current_section, "Format")){
		for(int i = 0; i < DOU_COUNT; i++){
			size_t size = 128;
			char buf[size];
			char *base = e_dou_to_str(i); 
			t_snprintf(buf, size, NULL, "%s%s", base, "Format");	
			if(!t_check(p.key, buf)){continue;}
			t_cpy(&s->dou_formats[i], p.value);
		}
	}
	if(t_check(p.current_section, "Instances")){
		for(int i = 0; i < INSTANCE_ENUM_COUNT; i++){
			size_t size = 128;
			char buf[size];
			char *base = e_dou_to_str(i); 
			t_snprintf(buf, size, NULL, "%s%s", base, "Count");	
			if(!t_check(p.key, buf)){continue;}
			t_atoi(p.value, &s->instance_counts[i]);
		}
	}
	if(t_check(p.current_section, "Animation")){
		if(t_check(p.key, "AnimationFps")){
			t_atof(p.value, &s->anim_fps);
		}
	}
}
