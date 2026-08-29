#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "t_math.h"
#include "e_map_manager.h"

#include "t_log_handler.h"

#include "si_map.h"

struct MapManager *e_create_map_manager(int map_index){
	struct MapManager *map_manager = XCALLOC(1, sizeof(struct MapManager));
	map_manager->map_pack = si_load_map(map_index);
	int *meta = map_manager->map_pack->metadata;
	size_t map_size = meta[M_WIDTH] * meta[M_HEIGHT] * sizeof(bool);
	map_manager->occupancy = XCALLOC(1, map_size);		
	memcpy(map_manager->occupancy, map_manager->map_pack->occupancy, map_size);	
	return map_manager;
}
void e_free_map_manager(struct MapManager *map_manager){
	if(!map_manager){return;}
	if(map_manager->occupancy){free(map_manager->occupancy);}
	si_free_map(map_manager->map_pack);
	map_manager->occupancy = NULL;
	free(map_manager);
	map_manager = NULL;
}
void e_occupy(struct MapManager *man, v2 tile){
	if(!man){return;}
	int indx = tile.y * man->map_pack->metadata[M_WIDTH] + tile.x;
	if(man->occupancy){man->occupancy[indx] = true;}
}
void e_tclear(struct MapManager *man, v2 tile){
	if(!man){return;}
	int indx = tile.y * man->map_pack->metadata[M_WIDTH] + tile.x;
	if(man->occupancy){man->occupancy[indx] = false;}
}
