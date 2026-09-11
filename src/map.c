#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "util/util.h"
#include "map.h"

#define DEFAULT_SIZE 1
#define MAP_MAGIC "LoneStar"
#define MAP_VERSION 2 // bumped: struct Sprite gained a `dir` byte
#define MAP_ENDIAN_CHECK 0x0102

struct Map *create_map(int width, int height, int map_gindx){
	if(width <= 0){LOG(INDEX, "Cann't create map of width smaller or equal to 0");return NULL;}
	if(height <= 0){LOG(INDEX, "Cannot create map of height smaller or equal to 0");return NULL;}

	struct Map *m = XCALLOC(1, sizeof(struct Map));
	m->width = (tilsize)width;
	m->height = (tilsize)height;
	m->map_gindx = (indsize)map_gindx;

	m->sprite_count = 0;
	m->sprite = NULL;

	m->entity_count = 0;
	m->entity = NULL;

	m->exit_count = 0;
	m->exit = NULL;

	m->tile_flag = XCALLOC((size_t)width * (size_t)height, sizeof(tile_flag_size));
	return m;
};
void free_map(struct Map *m){
	if(!m){LOG(IS_NULL, "Map is NULL");return;}
	if(m->sprite){free(m->sprite);m->sprite = NULL;}
	if(m->entity){free(m->entity);m->entity = NULL;}
	if(m->exit){free(m->exit);m->exit = NULL;}
	if(m->tile_flag){free(m->tile_flag);m->tile_flag = NULL;}
	free(m);
	m = NULL;
}

/*
 * Per-element byte swap helpers. We can't just swap a whole struct's raw
 * bytes at once, since it's made of fields of different widths - each
 * field has to be swapped on its own terms.
 */
static void swap_sprite(struct Sprite *s){
	s->sprite_gindex = swap16(s->sprite_gindex);
	s->tx = swap16(s->tx);
	s->ty = swap16(s->ty);
	// s->type is a single byte (uint8_t) - nothing to swap
}
static void swap_entity(struct Entity *e){
	e->prototype_gindex = swap16(e->prototype_gindex);
	e->instance_gindex  = swap16(e->instance_gindex);
	e->tx = swap16(e->tx);
	e->ty = swap16(e->ty);
	e->flags = swap32(e->flags);
	// e->dir is a single byte (uint8_t) - nothing to swap
}
static void swap_exit(struct ExitTile *e){
	e->map_gindx = swap16(e->map_gindx);
	e->tx = swap16(e->tx);
	e->ty = swap16(e->ty);
}

void write_map(struct Map *m, char *path){
	if(!m){LOG(IS_NULL, "Map is NULL");return;}
	if(!path){LOG(IS_NULL, "Path is NULL");return;}

	FILE *f = fopen(path, "wb");
	if(!f){LOG(NO_FILE, "Could not open %s for writing", path);return;}

	// We always write in the host's native byte order and stamp
	// MAP_ENDIAN_CHECK as-is. r() detects a mismatch on load and does
	// the byte-flipping there, so writing stays simple.
	struct Header h = {0};
	memcpy(&h.magic_number, MAP_MAGIC, sizeof(h.magic_number));
	h.time_stamp = (uint32_t)time(NULL);
	h.endian_check = MAP_ENDIAN_CHECK;
	h.version = MAP_VERSION;

	if(fwrite(&h, sizeof(h), 1, f) != 1){goto write_fail;}

	if(fwrite(&m->width, sizeof(m->width), 1, f) != 1){goto write_fail;}
	if(fwrite(&m->height, sizeof(m->height), 1, f) != 1){goto write_fail;}
	if(fwrite(&m->map_gindx, sizeof(m->map_gindx), 1, f) != 1){goto write_fail;}
	if(fwrite(&m->sprite_count, sizeof(m->sprite_count), 1, f) != 1){goto write_fail;}
	if(fwrite(&m->entity_count, sizeof(m->entity_count), 1, f) != 1){goto write_fail;}
	if(fwrite(&m->exit_count, sizeof(m->exit_count), 1, f) != 1){goto write_fail;}

	if(m->sprite_count > 0 && fwrite(m->sprite, sizeof(struct Sprite), m->sprite_count, f) != m->sprite_count){goto write_fail;}
	if(m->entity_count > 0 && fwrite(m->entity, sizeof(struct Entity), m->entity_count, f) != m->entity_count){goto write_fail;}
	if(m->exit_count > 0 && fwrite(m->exit, sizeof(struct ExitTile), m->exit_count, f) != m->exit_count){goto write_fail;}

	{
		size_t tile_count = (size_t)m->width * (size_t)m->height;
		if(tile_count > 0 && fwrite(m->tile_flag, sizeof(tile_flag_size), tile_count, f) != tile_count){goto write_fail;}
	}

	fclose(f);
	return;

write_fail:
	LOG(WRITE, "Failed to write map data to %s", path);
	fclose(f);
}

struct Map *read_map(char *path){
	if(!path){LOG(IS_NULL, "Path is NULL");return NULL;}

	FILE *f = fopen(path, "rb");
	if(!f){LOG(NO_FILE, "Could not open %s for reading", path);return NULL;}

	struct Header h = {0};
	if(fread(&h, sizeof(h), 1, f) != 1){
		LOG(READ, "Failed to read header from %s", path);
		fclose(f);
		return NULL;
	}

	// The magic number is copied in/out as raw bytes (never interpreted
	// as a number), so it compares equal regardless of byte order.
	uint64_t expected_magic = 0;
	memcpy(&expected_magic, MAP_MAGIC, sizeof(expected_magic));
	if(h.magic_number != expected_magic){
		LOG(PARSE, "%s is not a valid map file (bad magic number)", path);
		fclose(f);
		return NULL;
	}

	// endian_check IS interpreted as a number, so this is where we can
	// tell whether the file was written on a machine with the opposite
	// byte order. If so, flip every remaining multi-byte field as we
	// read it, instead of refusing to load the file.
	bool need_swap = false;
	if(h.endian_check == MAP_ENDIAN_CHECK){
		need_swap = false;
	}else if(swap16(h.endian_check) == MAP_ENDIAN_CHECK){
		need_swap = true;
		h.version = swap16(h.version);
		h.time_stamp = swap32(h.time_stamp);
	}else{
		LOG(PARSE, "%s has a corrupt or unrecognized endian marker", path);
		fclose(f);
		return NULL;
	}

	if(h.version != MAP_VERSION){
		LOG(PARSE, "%s has unsupported map version %u (expected %u)", path, h.version, MAP_VERSION);
		fclose(f);
		return NULL;
	}

	tilsize width, height;
	indsize map_gindx;
	dsize sprite_count, entity_count, exit_count;

	if(fread(&width, sizeof(width), 1, f) != 1){goto read_fail;}
	if(fread(&height, sizeof(height), 1, f) != 1){goto read_fail;}
	if(fread(&map_gindx, sizeof(map_gindx), 1, f) != 1){goto read_fail;}
	if(fread(&sprite_count, sizeof(sprite_count), 1, f) != 1){goto read_fail;}
	if(fread(&entity_count, sizeof(entity_count), 1, f) != 1){goto read_fail;}
	if(fread(&exit_count, sizeof(exit_count), 1, f) != 1){goto read_fail;}

	if(need_swap){
		width = swap16(width);
		height = swap16(height);
		map_gindx = swap16(map_gindx);
		sprite_count = swap16(sprite_count);
		entity_count = swap16(entity_count);
		exit_count = swap16(exit_count);
	}

	if(width == 0 || height == 0){
		LOG(PARSE, "%s has invalid dimensions (%u x %u)", path, width, height);
		fclose(f);
		return NULL;
	}

	struct Map *m = XCALLOC(1, sizeof(struct Map));

	m->width = width;
	m->height = height;
	m->map_gindx = map_gindx;
	m->sprite_count = sprite_count;
	m->entity_count = entity_count;
	m->exit_count = exit_count;

	if(sprite_count > 0){
		m->sprite = XCALLOC(sprite_count, sizeof(struct Sprite));
		if(fread(m->sprite, sizeof(struct Sprite), sprite_count, f) != sprite_count){goto read_fail_alloc;}
		if(need_swap){
			for(dsize i = 0; i < sprite_count; i++){swap_sprite(&m->sprite[i]);}
		}
	}
	if(entity_count > 0){
		m->entity = XCALLOC(entity_count, sizeof(struct Entity));
		if(fread(m->entity, sizeof(struct Entity), entity_count, f) != entity_count){goto read_fail_alloc;}
		if(need_swap){
			for(dsize i = 0; i < entity_count; i++){swap_entity(&m->entity[i]);}
		}
	}
	if(exit_count > 0){
		m->exit = XCALLOC(exit_count, sizeof(struct ExitTile));
		if(fread(m->exit, sizeof(struct ExitTile), exit_count, f) != exit_count){goto read_fail_alloc;}
		if(need_swap){
			for(dsize i = 0; i < exit_count; i++){swap_exit(&m->exit[i]);}
		}
	}
	{
		size_t tile_count = (size_t)width * (size_t)height;
		m->tile_flag = XCALLOC(tile_count, sizeof(tile_flag_size));
		if(fread(m->tile_flag, sizeof(tile_flag_size), tile_count, f) != tile_count){goto read_fail_alloc;}
		if(need_swap){
			for(size_t i = 0; i < tile_count; i++){m->tile_flag[i] = swap16(m->tile_flag[i]);}
		}
	}

	fclose(f);
	return m;

read_fail_alloc:
	LOG(PARSE, "%s is truncated or corrupt", path);
	free_map(m);
	fclose(f);
	return NULL;

read_fail:
	LOG(PARSE, "%s is truncated or corrupt", path);
	fclose(f);
	return NULL;
}
