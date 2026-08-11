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
	T_IS_CORNER,
	T_WALL_COLLIDE,
	T_FLOOR_COLLIDE,
	T_ALWAYS_ABOVE,
	T_MERGE_WALL,
	T_MERGE_FLOOR,
	T_HIDE_IF_ABOVE,
};
// This is specifically 
// signifying the outward facing plane
// dir is passed straight to l_draw_sprite as the FRAME INDEX, for both the
// corner row and the straight row, so this order must match the sheet's
// left-to-right frame order. It does.
enum WallDirections{
	W_NORTH,
	W_SOUTH,
	W_EAST,
	W_WEST,
	W_DIR_COUNT,
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
	enum WallDirections dir;
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
struct MapEntityData{
	int gindx;
	int world_spawn_x;
	int world_spawn_y;
	int GUID;
};

struct MapMetadata{
	int meta[M_META_COUNT];	
};
struct MapRuntime{
	struct MapEntityData *e; // Entity pos and info unconfided to the grid
	struct MapInteractableData *i; // Doors traps puzzels confined to the grid etc.
	struct MapDecompTile *t; // Walls ceilings and floors confined to the grid
	struct MapSegmentData *s;
};

struct MapPack{
	struct MapMetadata m;
	struct MapRuntime d;

};

bool m_free_map(int gindx);
bool m_load_map(int gindx);
struct MapPack m_get_map(int gindx, bool autoload);
