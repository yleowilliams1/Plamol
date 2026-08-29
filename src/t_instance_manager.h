#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "t_math.h"
#include "t_depot_manager.h"

#include "depo_sprite.h"


typedef void (*Update)(void *slot, float delta);
typedef void (*Draw)(void *slot, float delta);
typedef void (*Interact)(void *slot, void *message);
typedef void (*Serialize)(void *slot, FILE *file);
typedef void (*Deserialize)(void *slot, FILE *file);
typedef void (*Free)(void *slot);
#pragma pack (push, 1)
struct InstanceHeader{
	uint32_t time_stamp;
	uint64_t magic_number;
	uint16_t endian_check;
	uint16_t version;
};
#pragma pack(pop)

struct InstanceFunctions{
	Update on_update;
	Draw on_draw;
	Interact on_interact;
	Serialize on_serialize;
	Deserialize on_deserialize;
	Free on_free;
};
struct Instance{
	void *prototype_copy;
	
	bool is_global_coordinates;
	bool can_passthrough;

	v3 pos;
	
	struct AnimationState anim;
	
	enum Direction facing;
};
struct InstanceManager{
	struct Instance **instances;
	struct InstanceFunctions fncs;
	int count;
	size_t prototype_size;
};

