#pragma once
#include <stdbool.h>
#include "c_flag_enums.h"


struct EngineSettings{
	char *si_paths[SI_COUNT];
	char *proto_paths[PROT_COUNT];
	char *proto_formats[PROT_COUNT];
	int proto_item_counts[PROT_COUNT]; 
	int instance_counts[INSTANCE_COUNT];
	float anim_fps;
};

void e_free_settings();
void e_load_engine_settings();
char *e_grab_sipath(enum SiFlag si);
int e_grab_inscount(enum InstanceFlag type);
float e_grab_animfps();

char *e_grab_protopath(enum PrototypeFlag proto);
char *e_grab_protoformat(enum PrototypeFlag proto);
int e_grab_proto_itemcount(enum PrototypeFlag proto);
