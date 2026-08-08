#pragma once
#include <stdbool.h>
#include "p_entity.h"
#include "i_inventory.h"
#include "p_stats.h"

enum Dev;

struct MapData;

struct EntityInstance{
	int entity_gindx;
	int GUID;
	bool valid;
	bool dirty;
	struct Entity e;
	struct Inventory i;
	struct BaseStats s;	
};

bool p_instantiate_entities(struct MapData m);
bool p_evict_entity(int persistent_id);
bool p_derive_mutablestats(int gindx, enum Dev d, bool autoload, int *out);
