#include <stdio.h>
#include "t_math.h"
#include "e_map_manager.h"

#include "t_log_handler.h"

#include "si_map.h"


struct MapManager *e_create_map_manager(int map_index){
	struct MapManager *map_manager = XCALLOC(1, sizeof(struct MapManager));
	map_manager->map_pack = si_load_map(map_index);
	map_manager->occupancy = XCALLOC(1, sizeof(struct Occupant) * map_manager->map_pack->metadata[M_ENTITY_COUNT]);	
	
	return map_manager;
}
void e_free_map_manager(struct MapManager *map_manager){
	if(!map_manager){return;}
	if(map_manager->occupancy){free(map_manager->occupancy);}
	map_manager->occupancy = NULL;
	free(map_manager);
	map_manager = NULL;
}
