#pragma once
#include <stdbool.h>

#include "c_reg_list.h"
#include "c_si_list.h"

struct EngineSettings{
	float anim_fps;
	
	char *si_paths[SI_COUNT];
	
	char *reg_paths[REGISTER_COUNT];
	char *reg_formats[REGISTER_COUNT];
	int   reg_item_counts[REGISTER_COUNT];
};

void e_free_settings();
void e_load_engine_settings();

char *e_grab_sipath(enum SiEnum si);
float e_grab_animfps();

char *e_grab_regpath(enum RegisterType reg);
char *e_grab_regformat(enum RegisterType reg);
int   e_grab_regitemcount(enum RegisterType reg);

