#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e_engine_settings.h"

#include "t_config_tool.h"
#include "t_log_handler.h"
#include "t_strings.h"

#include "c_flag_enums.h"

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
	for(int i = 0; i < PROT_COUNT; i++){
		if(settings->proto_paths[i]){free(settings->proto_paths[i]);}
		if(settings->proto_formats[i]){free(settings->proto_formats[i]);}
		settings->proto_item_counts[i] = 0;
		settings->proto_paths[i] = NULL;
		settings->proto_formats[i] = NULL;
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
			LOG(LOG_NULL, "%s didn't load and is currently NULL", si_flag_str(i));
		}
	}
}

char *e_grab_protopath(enum PrototypeFlag proto){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return NULL;}
	return settings->proto_paths[proto];
}
char *e_grab_protoformat(enum PrototypeFlag proto){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return NULL;}
	return settings->proto_formats[proto];
}
int e_grab_proto_itemcount(enum PrototypeFlag proto){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return 1;}
	return settings->proto_item_counts[proto];
}
char *e_grab_sipath(enum SiFlag si){
	if(!settings){LOG(LOG_NULL, "Settings is NULL");return NULL;}
	return settings->si_paths[si];	
}
int e_grab_inscount(enum InstanceFlag type){
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
			if(!t_check(p.key, si_flag_str(i))){continue;}
			t_cpy(&s->si_paths[i], p.value);
		}
		for(int i = 0; i < PROT_COUNT; i++){
			if(!t_check(p.key, protflag_str(i))){continue;}
			t_cpy(&s->proto_paths[i], p.value);
		}
	}
	if(t_check(p.current_section, "Memory")){
		for(int i = 0; i < PROT_COUNT; i++){
			size_t size = 128;
			char buf[size];
			char *base = protflag_str(i); 
			t_snprintf(buf, size, NULL, "%s%s", base, "Count");	
			if(!t_check(p.key, buf)){continue;}
			t_atoi(p.value, &s->proto_item_counts[i]);
		}
	}
	if(t_check(p.current_section, "Format")){
		for(int i = 0; i < PROT_COUNT; i++){
			size_t size = 128;
			char buf[size];
			char *base = protflag_str(i); 
			t_snprintf(buf, size, NULL, "%s%s", base, "Format");	
			if(!t_check(p.key, buf)){continue;}
			t_cpy(&s->proto_formats[i], p.value);
		}
	}
	if(t_check(p.current_section, "Instances")){
		for(int i = 0; i < INSTANCE_COUNT; i++){
			size_t size = 128;
			char buf[size];
			char *base = protflag_str(i); 
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
