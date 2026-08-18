#pragma once
#include "t_math.h"
#include <stdint.h>
#include <stdbool.h>

enum SegmentFlags{
	INVISIBLE, // Visiblity
	IS_WALLS, // Means from start to end is all walls
	WALL_IS_NORTH,// Means is walls will draw wall facing north
	WALL_IS_SOUTH,// Means is walls will draw wall facing south
	WALL_IS_EAST,// Means is walls will draw wall facing east
	WALL_IS_WEST, // Means is walls will draw wall facing west
	HAS_WALLS_REC,// Means the edge tiles of the rectangle have walls
	HAS_FLOORS_REC, // Means rec is filled with tiles
	IS_WALLS_COLLIDE, // Toggles if walls collide
	IS_FLOOR_COLLIDE, // Toggles if floors collide
	ALWAYS_ABOVE_PLAYER, // Always draws above player
	SHOULD_MERGE_WALL,// Smoothly transition between other segments neighbouring walls
       	SHOULD_MERGE_FLOOR, // Smoothly transition between other segments neighbouring floors
	HIDE_IF_ABOVE_PLAYER, // Don't draw if the player is below this segment (second floors etc)
	SEGMENT_FLAGS_COUNT,
};
enum TileFlags{
	T_INVISIBLE,
	T_HAS_WALL,
	T_HAS_TILE,
	T_HAS_INTERACTABLE,
	T_WALL_COLLIDE,
	T_FLOOR_COLLIDE,
	T_ALWAYS_ABOVE,
	T_MERGE_WALL,
	T_MERGE_FLOOR,
	T_HIDE_IF_ABOVE,
};
enum TileDirections{
	W_NORTH,
	W_SOUTH,
	W_EAST,
	W_WEST,
	W_NORTH_EAST,
	W_SOUTH_WEST,
	W_NORTH_WEST,
	W_SOUTH_EAST,
	W_DIR_COUNT,
};
enum MetadataProperties{
	M_NORTH_EXIT,
	M_SOUTH_EXIT,
	M_WEST_EXIT,
	M_EAST_EXIT,
	
	M_ENTITY_COUNT,
	M_INTERACTABLE_COUNT,
	M_SEGMENT_COUNT,

	M_WIDTH,
	M_HEIGHT,	
	
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
	enum TileDirections dir;
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
	enum TileDirections dir;
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
