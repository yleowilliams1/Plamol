#pragma once
#include <stdbool.h>
#include "e_dou_manager.h"

enum SiEnum{
	SI_FLAGS,
	SI_STATE,
	SI_INPUT,
	SI_MAP,
	SI_GAMESTATE,
	SI_COUNT,
};

struct EngineSettings{
	char *si_paths[SI_COUNT];
	char *dou_paths[DOU_COUNT];
	char *dou_formats[DOU_COUNT];
	int dou_icounts[DOU_COUNT]; 
};

char *e_si_to_str(enum SiEnum type);
void e_free_settings();
void e_load_engine_settings();
char *e_grab_sipath(enum SiEnum si);
char *e_grab_doupath(enum DouEnum dou);
char *e_grab_douformat(enum DouEnum dou);
int e_grab_doucount(enum DouEnum dou);
