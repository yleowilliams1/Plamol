#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <raylib.h>

#include "util/util.h"
#include "sprite.h"
#include "map.h"
#include "settings.h"

#define ANIMATION_COUNT_INVALID -1

// Parser used to check animation_count against ANIMATION_COUNT_INVALID to
// enforce ini ordering: [metadata] (which sets animation_count) must come
// before [frame_counts], which must come before [frame_origins], since each
// section's parsing depends on arrays sized by the one before it.
static void parse_sprite(struct config_pack p, void *ptr){
	struct SpriteData *spr = (struct SpriteData *)ptr;
	if(!spr){LOG(IS_NULL, "Passed NULL pointer to sprite parser"); return;}

	if(check(p.current_section, "metadata")){
		if(spr->origin){LOG(ABORT, "Cannot have metadata placed after origin declaration!"); return;}
		if(check(p.key, "sprite_path")){
			t_cpy(p.value, &spr->sprite_path);
		}
		if(check(p.key, "animation_count")){
			if(spr->animation_count != ANIMATION_COUNT_INVALID){return;}
			t_atoi(p.value, &spr->animation_count);
			if(spr->animation_count <= 0){
				LOG(ABORT, "%d is not a valid animation_count", spr->animation_count);
				return;
			}
		}
		return;
	}

	if(check(p.current_section, "frame_counts")){
		if(spr->animation_count == ANIMATION_COUNT_INVALID){
			LOG(ABORT, "animation_count (set in [metadata]) must come before [frame_counts]. Fix the ini order.");
			return;
		}
		if(spr->origin){LOG(ABORT, "Can't have frame_counts placed after origin declaration!"); return;}
		if(!spr->frame_count){spr->frame_count = XCALLOC((size_t)spr->animation_count, sizeof(int));}
		for(int i=0; i<spr->animation_count; i++){
			char buf[64];
			if(!t_snprintf(buf, sizeof(buf), NULL, "animation[%d].frame_count", i)){
				LOG(IS_NULL, "Failed to t_snprintf"); return;
			}
			if(check(p.key, buf)){
				t_atoi(p.value, &spr->frame_count[i]);
			}
		}
		return;
	}

	if(check(p.current_section, "frame_origins")){
		if(spr->animation_count == ANIMATION_COUNT_INVALID){
			LOG(ABORT, "animation_count (set in [metadata]) must come before [frame_origins]. Fix the ini order.");
			return;
		}
		if(!spr->frame_count){
			LOG(ABORT, "frame_counts must be set before [frame_origins]. Fix the ini order.");
			return;
		}
		if(!spr->origin){
			for(int i=0; i<spr->animation_count; i++){
				if(spr->frame_count[i] <= 0){
					LOG(ABORT, "Animation %d has an invalid frame_count of %d", i, spr->frame_count[i]);
					return;
				}
			}
			spr->origin = XCALLOC((size_t)spr->animation_count, sizeof(v2 *));
			for(int i=0; i<spr->animation_count; i++){
				spr->origin[i] = XCALLOC((size_t)spr->frame_count[i], sizeof(v2));
			}
		}
		for(int anim=0; anim<spr->animation_count; anim++){
			for(int frame=0; frame<spr->frame_count[anim]; frame++){
				char buf_x[64];
				if(!t_snprintf(buf_x, sizeof(buf_x), NULL, "anim[%d].frame[%d].x", anim, frame)){
					LOG(IS_NULL, "Failed to t_snprintf"); return;
				}
				if(check(p.key, buf_x)){t_atoi(p.value, &spr->origin[anim][frame].x);}

				char buf_y[64];
				if(!t_snprintf(buf_y, sizeof(buf_y), NULL, "anim[%d].frame[%d].y", anim, frame)){
					LOG(IS_NULL, "Failed to t_snprintf"); return;
				}
				if(check(p.key, buf_y)){t_atoi(p.value, &spr->origin[anim][frame].y);}
			}
		}
	}
}

struct SpriteData *load_sprite_from_disk(int sprite_gindex){
	char *path = format_path(STR(SPRITE_PATH), "cfg", sprite_gindex);
	if(!path){
		LOG(IS_NULL, "Failed to build sprite path for gindex %d", sprite_gindex);
		return NULL;
	}
	struct SpriteData *spr = XCALLOC(1, sizeof(struct SpriteData));
	spr->animation_count = ANIMATION_COUNT_INVALID; // see parse_sprite's ordering check
	spr->gindex = sprite_gindex;
	if(!config(spr, path, parse_sprite)){
		LOG(NO_FILE, "Failed to load sprite config at %s", path);
	}
	free(path);

	if(spr->sprite_path){
		spr->texture = LoadTexture(spr->sprite_path);
	}

	// Derive the per-frame pixel size from the sheet itself rather than
	// requiring it in the config: rows = animation_count, columns = the
	// widest animation's frame_count (animations with fewer frames just
	// leave the remaining columns in their row unused).
	if(spr->texture.width > 0 && spr->texture.height > 0 && spr->frame_count && spr->animation_count > 0){
		int max_frames = 0;
		for(int i=0; i<spr->animation_count; i++){
			if(spr->frame_count[i] > max_frames){max_frames = spr->frame_count[i];}
		}
		if(max_frames > 0){
			spr->frame_w = spr->texture.width  / max_frames;
			spr->frame_h = spr->texture.height / spr->animation_count;
		}
	}
	return spr;
}

struct SpriteManager *create_sprite_manager(){
	struct SpriteManager *sman = XCALLOC(1, sizeof(struct SpriteManager));
	sman->sprite = XCALLOC(INT(SPRITE_COUNT), sizeof(struct SpriteData *));
	return sman;
}

void free_sprite_manager(struct SpriteManager **sman){
	if(!sman || !(*sman)){return;}
	for(int i=0; i<INT(SPRITE_COUNT); i++){
		free_sprite_at(&(*sman)->sprite[i], i);
	}
	free((*sman)->sprite);
	(*sman)->sprite = NULL;
	free(*sman);
	*sman = NULL;
}

void load_map_sprites(struct SpriteManager *sman, struct Map *map){
	if(!sman || !map){return;}
	for(int i=0; i<map->sprite_count; i++){
		struct Sprite *s = &map->sprite[i];
		if(!sman->sprite[s->sprite_gindex]){
			sman->sprite[s->sprite_gindex] = load_sprite_from_disk(s->sprite_gindex);
		}
	}
}

void unload_map_sprites(struct SpriteManager *sman, struct Map *map){
	if(!sman || !map){return;}
	for(int i=0; i<map->sprite_count; i++){
		struct Sprite *s = &map->sprite[i];
		free_sprite_at(&sman->sprite[s->sprite_gindex], s->sprite_gindex);
	}
}

void free_sprite_at(struct SpriteData **s, int gindex){
	if(!*s){return;}
	struct SpriteData *spr = *s;

	if(spr->texture.id > 0){UnloadTexture(spr->texture);}

	if(spr->origin){
		for(int i=0; i<spr->animation_count; i++){
			if(spr->origin[i]){free(spr->origin[i]); spr->origin[i] = NULL;}
		}
		free(spr->origin);
		spr->origin = NULL;
	}
	if(spr->frame_count){free(spr->frame_count); spr->frame_count = NULL;}
	if(spr->sprite_path){free(spr->sprite_path); spr->sprite_path = NULL;}

	free(spr);
	*s = NULL;
}
