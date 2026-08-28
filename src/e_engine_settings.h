#pragma once
#include <stdbool.h>

#include "c_depot_list.h"
#include "c_instance_list.h"
#include "c_si_list.h"

struct EngineSettings{
	float anim_fps;
	int map_count;

	char *si_paths[SI_COUNT];
	
	char *depo_paths[DEPOT_COUNT];
	char *depo_formats[DEPOT_COUNT];
	int   depo_item_counts[DEPOT_COUNT];

	int instance_counts[INSTANCE_COUNT];
};

void e_free_settings();
void e_load_engine_settings();

int e_grab_mapcount();

char *e_grab_sipath(enum SiEnum si);
float e_grab_animfps();

int e_grab_instance_count(enum InstanceType instance);

char *e_grab_depopath(enum DepotType depot_index);
char *e_grab_depoformat(enum DepotType depot_index);
int   e_grab_depoitemcount(enum DepotType depot_index);

