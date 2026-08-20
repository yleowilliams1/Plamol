#pragma once
#include <stdbool.h>
#include "c_flag_enums.h"

struct EngineSettings{
	char *si_paths[SI_COUNT];
	char *dou_paths[DOU_COUNT];
	char *dou_formats[DOU_COUNT];
	int dou_icounts[DOU_COUNT]; 
	int instance_counts[INSTANCE_COUNT];
	float anim_fps;
};

void e_free_settings();
void e_load_engine_settings();
char *e_grab_sipath(enum SiFlag si);
char *e_grab_doupath(enum DouFlag dou);
char *e_grab_douformat(enum DouFlag dou);
int e_grab_doucap(enum DouFlag dou);
int e_grab_inscount(enum InstanceFlag type);
float e_grab_animfps();
