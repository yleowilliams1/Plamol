#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "t_math.h"
#include "depo_sprite.h"

#define META_PROPER\
	X(M_NORTH_EXIT)\
	X(M_SOUTH_EXIT)\
	X(M_WEST_EXIT) \
	X(M_EAST_EXIT)\
	X(M_SEGMENT_COUNT) \
	X(M_WIDTH) \
	X(M_HEIGHT) \
	X(M_GINDX) \
	X(M_ENTITY_INSTANCE_COUNT)\
	X(M_INTERACTABLE_INSTANCE_COUNT) \

enum MetadataProperties{
	#define X(name) name,
	META_PROPER
	#undef X	
	M_META_COUNT,
};

struct InstanceSlot{
	enum Direction direction;
	int prototype_gindx;
	int instance_gindx;
	v3 position;
	bool can_passthrough;
	bool is_global_coordinates;
};

#pragma pack (push, 1)
struct MapHeader{
	uint32_t time_stamp; // 4
	uint64_t magic_number; // 8
	uint16_t endian_check; // 2
	uint16_t version; // 2
	// 16 bytes
};
#pragma pack(pop)

struct MapSpriteSlot{
	int gindex;
	bool collide;
	v2 tile;
};
struct Map{
	int metadata[M_META_COUNT];

	struct InstanceSlot *entity_instances;
	struct InstanceSlot *interactable_instances;
	
	int *ground_sprite_gindex;
	int *wall_sprite_gindex;
	int *ceiling_sprite_gindex;
	bool *occupancy;
};

struct Map *si_load_map(int gindx);
void si_free_map(struct Map *map);

void si_write_map_to_disk(struct Map *map, const char *filename);
struct Map *si_read_map_to_memory(const char *filename);
const char *mmetastr(enum MetadataProperties type);
