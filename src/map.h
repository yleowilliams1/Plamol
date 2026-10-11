#pragma once
#include <stdint.h>
#include <stdio.h>

#define TYPE\
	X(GROUND)\
	X(WALL)\
	X(PROP)\
	X(CEILING)
#define DIRECTION\
	X(NW)\
	X(SW)\
	X(SE)\
	X(NE)\
	X(N)\
	X(W)\
	X(S)\
	X(E)
#define TILE_FLAGS\
	X(COLLIDE)\
	X(DIRT_SOUND)\
	X(GRASS_SOUND)\
	X(STONE_SOUND)
enum Dir{
	#define X(name) name,
	DIRECTION
	#undef X
	DIR_COUNT,
};
enum Type{
	#define X(name) name,
	TYPE
	#undef X
};
enum TileFlags{
	#define X(name) name,
	TILE_FLAGS
	#undef X
};
struct Sprite{
	uint32_t tx, ty;
	uint32_t sprite_index;
	uint8_t type; 
	uint8_t dir;  //which origin row (SpriteData->origin[dir]) this decoration uses
};
struct Entity{
	uint8_t dir; 
	uint32_t prototype_gindex;
	uint32_t instance_gindex;
	uint32_t tx, ty;
	uint32_t flags;
};
struct ExitTile{
	uint32_t map_gindx;
	uint32_t tx, ty;
};
struct Header{
	uint64_t magic_number;
	uint32_t time_stamp;
	uint16_t endian_check;
	uint16_t version;
};
struct Map{
	struct Sprite *sprite;
	struct Entity *entity;
	struct ExitTile *exit;
	uint32_t sprite_count;
	uint32_t entity_count;
	uint32_t exit_count;
	uint32_t width, height;
	uint32_t map_gindx;
	uint32_t *tile_flag;
};
struct Map *create_map(int width, int height, int map_gindx);
void free_map(struct Map *m);

void write_map(struct Map *m, char *path);
struct Map *read_map(char *path);
