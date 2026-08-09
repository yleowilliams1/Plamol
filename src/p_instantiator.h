#pragma once
#include <stdbool.h>
#include "p_entity.h"
#include "i_inventory.h"
#include "p_stats.h"

enum Dev;

struct MapData;

struct EntityInstance *grab_entity(int GUID);
int grab_entity_pool_size();
bool p_instantiate_entities(struct MapData m);
struct EntityInstance *grab_entity_pool();
bool p_clear_entity_pool();
