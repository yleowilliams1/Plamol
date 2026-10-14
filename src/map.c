#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "util/util.h"
#include "map.h"

#define DEFAULT_SIZE 1
#define MAP_MAGIC "LoneStar"
#define MAP_VERSION 1
// Bytes per record on disk. This is the sum of the fields written below, NOT sizeof(struct), because structs have padding.
#define SPRITE_DISK_SIZE 14  // tx ty sprite_index (3 x u32) + type dir (2 x u8)
#define ENTITY_DISK_SIZE 21  // dir (u8) + prototype_gindex instance_gindex tx ty flags (5 x u32)
#define EXIT_DISK_SIZE   12  // map_gindx tx ty (3 x u32)
// This thing is immune to changes. I'd bet my left leg that this doesn't change from this commit
struct Map *create_map(int width, int height, int map_gindx){
	if(width <= 0){LOG(INDEX, "Cann't create map of width smaller or equal to 0");return NULL;}
	if(height <= 0){LOG(INDEX, "Cannot create map of height smaller or equal to 0");return NULL;}

	struct Map *m = XCALLOC(1, sizeof(struct Map));
	m->width = (uint32_t)width;
	m->height = (uint32_t)height;
	m->map_gindx = (uint32_t)map_gindx;

	m->sprite_count = 0;
	m->sprite = NULL;

	m->entity_count = 0;
	m->entity = NULL;

	m->exit_count = 0;
	m->exit = NULL;

	m->tile_flag = XCALLOC((size_t)width * (size_t)height, sizeof(uint32_t));
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

static bool write_sprite(FILE *f, const struct Sprite *s){
	return write_u32(f, s->tx) && write_u32(f, s->ty) && write_u32(f, s->sprite_index)
	    && write_u8(f, s->type) && write_u8(f, s->dir);
}
static bool read_sprite(FILE *f, struct Sprite *s){
	return read_u32(f, &s->tx) && read_u32(f, &s->ty) && read_u32(f, &s->sprite_index)
	    && read_u8(f, &s->type) && read_u8(f, &s->dir);
}
static bool write_entity(FILE *f, const struct Entity *e){
	return write_u8(f, e->dir) && write_u32(f, e->prototype_gindex) && write_u32(f, e->instance_gindex)
	    && write_u32(f, e->tx) && write_u32(f, e->ty) && write_u32(f, e->flags);
}
static bool read_entity(FILE *f, struct Entity *e){
	return read_u8(f, &e->dir) && read_u32(f, &e->prototype_gindex) && read_u32(f, &e->instance_gindex)
	    && read_u32(f, &e->tx) && read_u32(f, &e->ty) && read_u32(f, &e->flags);
}
static bool write_exit(FILE *f, const struct ExitTile *e){
	return write_u32(f, e->map_gindx) && write_u32(f, e->tx) && write_u32(f, e->ty);
}
static bool read_exit(FILE *f, struct ExitTile *e){
	return read_u32(f, &e->map_gindx) && read_u32(f, &e->tx) && read_u32(f, &e->ty);
}

bool write_map(struct Map *m, char *path){
	if(!m){LOG(IS_NULL, "Map is NULL");return false;}
	if(!path){LOG(IS_NULL, "Path is NULL");return false;}
	if(m->sprite_count > 0 && !m->sprite){LOG(IS_NULL, "Map has a sprite count but no sprites");return false;}
	if(m->entity_count > 0 && !m->entity){LOG(IS_NULL, "Map has an entity count but no entities");return false;}
	if(m->exit_count > 0 && !m->exit){LOG(IS_NULL, "Map has an exit count but no exits");return false;}
	if(!m->tile_flag){LOG(IS_NULL, "Map has no tile flags");return false;}

	FILE *f = fopen(path, "wb");
	if(!f){LOG(NO_FILE, "Could not open %s for writing", path);return false;}

	struct Header h = create_header(MAP_MAGIC, MAP_VERSION);
	if(!write_header(f, &h)){goto write_fail;}
	if(!write_u32(f, m->width)){goto write_fail;}
	if(!write_u32(f, m->height)){goto write_fail;}
	if(!write_u32(f, m->map_gindx)){goto write_fail;}
	if(!write_u32(f, m->sprite_count)){goto write_fail;}
	if(!write_u32(f, m->entity_count)){goto write_fail;}
	if(!write_u32(f, m->exit_count)){goto write_fail;}

	for(uint32_t i = 0; i < m->sprite_count; i++){
		if(!write_sprite(f, &m->sprite[i])){goto write_fail;}
	}
	for(uint32_t i = 0; i < m->entity_count; i++){
		if(!write_entity(f, &m->entity[i])){goto write_fail;}
	}
	for(uint32_t i = 0; i < m->exit_count; i++){
		if(!write_exit(f, &m->exit[i])){goto write_fail;}
	}
	{
		size_t tile_count = (size_t)m->width * (size_t)m->height;
		for(size_t i = 0; i < tile_count; i++){
			if(!write_u32(f, m->tile_flag[i])){goto write_fail;}
		}
	}

	if(fclose(f) != 0){f = NULL; goto write_fail;}
	return true;

write_fail:
	LOG(WRITE, "Failed to write map data to %s", path);
	if(f){fclose(f);}
	return false;
}

struct Map *read_map(char *path){
	if(!path){LOG(IS_NULL, "Path is NULL");return NULL;}

	FILE *f = fopen(path, "rb");
	if(!f){LOG(NO_FILE, "Could not open %s for reading", path);return NULL;}

	struct Map *m = NULL;
	struct Header h;
	uint32_t width, height, map_gindx;
	uint32_t sprite_count, entity_count, exit_count;
	size_t remaining = 0;

	if(!read_header(f, MAP_MAGIC, MAP_VERSION, &h)){goto fail;}

	if(!read_u32(f, &width)){goto fail;}
	if(!read_u32(f, &height)){goto fail;}
	if(!read_u32(f, &map_gindx)){goto fail;}
	if(!read_u32(f, &sprite_count)){goto fail;}
	if(!read_u32(f, &entity_count)){goto fail;}
	if(!read_u32(f, &exit_count)){goto fail;}

	if(width == 0 || height == 0){
		LOG(PARSE, "%s has invalid dimensions (%u x %u)", path, width, height);
		goto fail;
	}

	// Check the file is actually big enough for everything the header claims
	// before allocating anything. All of these products fit in a uint64_t
	// once tile_count has been bounded by the file size.
	if(!bytes_remaining(f, &remaining)){goto fail;}
	uint64_t tile_count = (uint64_t)width * (uint64_t)height;
	if(tile_count > remaining / sizeof(uint32_t)){
		LOG(PARSE, "%s claims a %u x %u map but is too short to hold it", path, width, height);
		goto fail;
	}
	uint64_t need = tile_count * sizeof(uint32_t)
	              + (uint64_t)sprite_count * SPRITE_DISK_SIZE
	              + (uint64_t)entity_count * ENTITY_DISK_SIZE
	              + (uint64_t)exit_count   * EXIT_DISK_SIZE;
	if(need > remaining){
		LOG(PARSE, "%s is too short for the object counts in its header", path);
		goto fail;
	}

	m = XCALLOC(1, sizeof(struct Map));
	m->width = width;
	m->height = height;
	m->map_gindx = map_gindx;
	m->sprite_count = sprite_count;
	m->entity_count = entity_count;
	m->exit_count = exit_count;

	if(sprite_count > 0){
		m->sprite = XCALLOC(sprite_count, sizeof(struct Sprite));
		for(uint32_t i = 0; i < sprite_count; i++){
			if(!read_sprite(f, &m->sprite[i])){goto fail;}
		}
	}
	if(entity_count > 0){
		m->entity = XCALLOC(entity_count, sizeof(struct Entity));
		for(uint32_t i = 0; i < entity_count; i++){
			if(!read_entity(f, &m->entity[i])){goto fail;}
		}
	}
	if(exit_count > 0){
		m->exit = XCALLOC(exit_count, sizeof(struct ExitTile));
		for(uint32_t i = 0; i < exit_count; i++){
			if(!read_exit(f, &m->exit[i])){goto fail;}
		}
	}
	m->tile_flag = XCALLOC((size_t)tile_count, sizeof(uint32_t));
	for(size_t i = 0; i < (size_t)tile_count; i++){
		if(!read_u32(f, &m->tile_flag[i])){goto fail;}
	}

	fclose(f);
	return m;

fail:
	LOG(READ, "Failed reading map %s", path);
	fclose(f);
	if(m){free_map(m);}
	return NULL;
}
