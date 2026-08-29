#pragma once
#include "t_depot_manager.h"
#include "t_instance_manager.h"
#include "e_map_manager.h"
#include "si_save_manager.h"

struct Stage{
	struct MapManager *map_manager;		
	struct DepotManager *depot_manager;
	struct SaveManager *save_manager;

	// Live, mutable instances for the current map. Spawned from the (immutable)
	// prototype depot via the map's InstanceSlot lists, or loaded straight from
	// a save file if the save manager has one active for this map/slot.
	struct InstanceManager *entity_instances;
	struct InstanceManager *interactable_instances;
};

struct Stage *si_init_stage(int map_index);
void si_free_stage(struct Stage *stage);
void si_draw_stage(struct Stage *stage);
