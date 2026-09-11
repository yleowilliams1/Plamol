#pragma once
#include <stdint.h>
#include <stdio.h>

/*
 * Fixed-width types used for anything that gets written to / read from
 * a map file. Plain `int` is NOT guaranteed to be the same size on every
 * compiler/platform, so it can't be trusted in a binary format. Everything
 * below is pinned to uint16_t so the file layout is identical everywhere.
 */
typedef uint16_t tilsize;        // width, height, tile (tx, ty) coordinates
typedef uint16_t indsize;        // gindex / index values
typedef uint16_t dsize;          // element counts (sprite/entity/exit)
typedef uint16_t tile_flag_size; // per-tile flag bitmask

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

/*
 * Structs below this line get fwrite()'d / fread()'d as raw bytes, so they're
 * packed to 1-byte alignment. Without this, two compilers (or the same
 * compiler with different flags) can insert different padding between
 * fields and produce differently-sized blobs for the "same" struct.
 *
 * `type`/`dir` are stored as uint8_t rather than `enum Type`/`enum Dir`
 * because an enum's underlying type is compiler-defined (commonly `int`,
 * sometimes 1 byte with -fshort-enums). uint8_t is plenty for either enum
 * and is guaranteed to be exactly 1 byte everywhere. Enum <-> uint8_t
 * conversions are implicit in C, so existing call sites that assign/compare
 * against the enum constants keep working unchanged.
 */
#pragma pack(push, 1)
struct Sprite{
	indsize sprite_gindex;
	tilsize tx, ty;
	uint8_t type; // enum Type
	uint8_t dir;  // enum Dir - which origin row (SpriteData->origin[dir]) this decoration uses
};
struct Entity{
	uint8_t dir; // enum Dir
	indsize prototype_gindex;
	indsize instance_gindex;
	tilsize tx, ty;
	uint32_t flags;
};
struct ExitTile{
	indsize map_gindx;
	tilsize tx, ty;
};
struct Header{
	uint64_t magic_number;
	uint32_t time_stamp;
	uint16_t endian_check;
	uint16_t version;
};
#pragma pack(pop)

struct Map{
	struct Sprite *sprite;
	struct Entity *entity;
	struct ExitTile *exit;
	dsize sprite_count;
	dsize entity_count;
	dsize exit_count;
	tilsize width, height;
	indsize map_gindx;
	tile_flag_size *tile_flag;
};

struct Map *create_map(int width, int height, int map_gindx);
void free_map(struct Map *m);

void write_map(struct Map *m, char *path);
struct Map *read_map(char *path);
