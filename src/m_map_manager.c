#include <stdio.h>
#include <stdlib.h>
#include "e_engine_settings.h"
#include "l_load_map.h"
#include "m_map_tile.h"
// Free dumbass
static struct MapData *map_ptr= NULL;

void m_init_map(){
	printf("If you call this before loading engine settings I will literally skin you I am not joking.\n");
	map_ptr = l_read_map(e_get_map_path());
}

void m_free_map(){
	if(map_ptr){free(map_ptr);}
}
