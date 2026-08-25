#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "t_math.h"
#include "depo_sprite.h"

#define SEG_FLAGS\
	X(INVISIBLE)\
	X(IS_WALLS)\
	X(WALL_IS_NORTH)\
	X(WALL_IS_SOUTH)\
	X(WALL_IS_EAST)\
	X(WALL_IS_WEST) \
	X(HAS_WALLS_REC)\
	X(HAS_FLOORS_REC) \
	X(IS_WALLS_COLLIDE) \
	X(IS_FLOOR_COLLIDE) \
	X(ALWAYS_ABOVE_PLAYER) \
	X(SHOULD_MERGE_WALL)\
       	X(SHOULD_MERGE_FLOOR) \
	X(HIDE_IF_ABOVE_PLAYER) \
	X(SEGMENT_FLAGS_COUNT)

enum SegmentFlags{
	#define X(name) name,
	SEG_FLAGS
	#undef X
	SEGMENT_FLAG_COUNT,
};

#define TILE_FLAGS \
	X(T_INVISIBLE) \
	X(T_HAS_WALL) \
	X(T_HAS_TILE) \
	X(T_HAS_INTERACTABLE) \
	X(T_WALL_COLLIDE) \
	X(T_FLOOR_COLLIDE) \
	X(T_ALWAYS_ABOVE) \
	X(T_MERGE_WALL) \
	X(T_MERGE_FLOOR) \
	X(T_HIDE_IF_ABOVE)
enum TileFlags{
	#define X(name) name,
	TILE_FLAGS
	#undef X
	TILE_FLAG_COUNT,
};

#define META_PROPER\
	X(M_NORTH_EXIT)\
	X(M_SOUTH_EXIT)\
	X(M_WEST_EXIT) \
	X(M_EAST_EXIT)\
	X(M_ENTITY_COUNT) \
	X(M_INTERACTABLE_COUNT) \
	X(M_SEGMENT_COUNT) \
	X(M_WIDTH) \
	X(M_HEIGHT) 

enum MetadataProperties{
	#define X(name) name,
	META_PROPER
	#undef X	
	M_META_COUNT,
};
struct MapSegmentData{
	uint32_t flags;	
	
	v2 start_tile;
	v2 end_tile;
	
	int z;

	int wall_gindx;
	int floor_gindx;
};
struct MapDecompTile{
	uint32_t flags;
	enum Direction dir;
	int wall_gindx;
	int wall_z;
	int floor_gindx;
	int floor_z;
	int interactable_gindx;
	int interactable_z;
};
struct MapInteractableData{
	v2 tile_position;
	int z;
	int gindx;
};
struct MapEntityData{
	int gindx;
	int tile_spawn_x;
	int tile_spawn_y;
	int GUID;
	enum Direction dir;
};

struct MapPack{
	int metadata[M_META_COUNT];
	struct MapEntityData *entities; 
	struct MapDecompTile *tiles; 
};
struct MapParsePackage{
	struct MapPack *map;
	struct MapSegmentData *segments;
	struct MapInteractableData *interactables; 
};
struct MapPack *si_load_map(int gindx);
bool si_free_map(struct MapPack *map);

const char *segflgstr(enum SegmentFlags type){
	switch(type){
		#define X(name) case name: return #name;
		SEG_FLAGS
		#undef X
		default: return NULL;
	}
}
const char *tileflgstr(enum TileFlags type){
	switch(type){
		#define X(name) case name: return #name;
		TILE_FLAGS
		#undef X
		default: return NULL;
	}
}
const char *mmetastr(enum MetadataProperties type){
	switch(type){
		#define X(name) case name: return #name;
		META_PROPER
		#undef X
		default: return NULL;
	}
}
