#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "util/util.h"
#include "inventory.h"
#include "stats.h"

// Eventually we'll do script hooks. But I'm working on the vs so it's not the highest priority. It shouldn't be too big a job.

struct SaveComponent{
	uint32_t uid;
	uint32_t flags;

	int32_t health_points;
	int32_t action_points;
	
	int32_t current_map;

	int32_t tx;
	int32_t ty;

	uint8_t direction;
	
	struct StatsSave stats_save;

	uint32_t level;
	struct Inventory inv;	
};
struct RuntimeComponent{
	vf2 tile_offset;
	int current_frame;
	float elapsed_time;
	
	// Movement	
	struct PathNode *path;
	size_t len;
	float time;
	int root_tile;
	int next_tile;
	vf2 offset_from_root;
};
struct ProtoComponent{
	int32_t uid;
	uint32_t name_string;
	uint32_t descr_string;	
	
	uint32_t sprite_id;
	int32_t dialogue_string;
	int32_t script_string;

	struct StatsProto stats_proto;
	struct Inventory inv;	

	// Used for interaction	
	int rec_offset_x;
	int rec_offset_y;
	int rec_width;
	int rec_height;
};

// How this works is that map store a proto and a save index for objects, and then create a object struct with that data which gets loaded from disk. And create a new runtime component. This is super simple. Then what happens is we look through the items and load the ones which show up in that data. 
struct Object{
	struct SaveComponent save;
	uint32_t prototype_index;
	struct RuntimeComponent runtime;
};	

struct EntityManager{
	/* We sacrificed the space of n*ptr for O(1) 
	 * complexity when we look for something. 
	 * Since we have our mutable and immutable
	 * gindexs; i.e just disk level data; we can
	 * load, unload, and get with just that index
	 * this way. And we keep unloaded data arr[gindex] = NULL*/
	
	struct Object *objects;
	size_t object_count;
	
	// This should be mostly NULL, then when we look at the map, and load the objects, after we loop through and look at all the items prottpyes which show up, and the ones which don't, and then unload those, and load the ones which do show up.
	struct ItemPrototype **item_proto;
	struct ProtoComponent **object_prototypes;
};
bool write_object(struct SaveComponent *c, uint32_t slot);
