#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include "draw.h"
#include "entity.h"
#include "hook.h"
#include "map.h"
#include "sprite.h"
#include "util/util.h"
#include "movement.h"
static int compare_draw_items(const void *a, const void *b);
static void draw_flat_sprites(struct Map *map, struct SpriteManager *sprite_manager, enum Type type);
static v2 sprite_origin_at(struct SpriteData *spr, int anim_index, int frame_index){
	v2 zero = {0, 0};
	if(!spr || !spr->origin || !spr->frame_count){return zero;}
	if(anim_index < 0 || anim_index >= spr->animation_count){
		LOG(INDEX, "Sprite %d: dir/anim index %d out of range (has %d rows) - drawing with zero origin", spr->gindex, anim_index, spr->animation_count);
		return zero;
	}
	if(frame_index < 0 || frame_index >= spr->frame_count[anim_index]){return zero;}
	return spr->origin[anim_index][frame_index];
}
static v2 entity_sprite_origin_at(struct SpriteData *spr, int anim_index, int direction, int frame_index){
	v2 zero = {0, 0};
	if(!spr || !spr->frame_count){return zero;}
	if(anim_index < 0 || anim_index >= spr->animation_count){return zero;}
	if(direction < 0 || direction >= DIR_COUNT){return zero;}

	int frames_per_dir = spr->frame_count[anim_index] / DIR_COUNT;
	if(frames_per_dir <= 0){return zero;}
	if(frame_index < 0 || frame_index >= frames_per_dir){return zero;}

	int real_frame = direction * frames_per_dir + frame_index;
	return sprite_origin_at(spr, anim_index, real_frame);
}
// Frames sit in a grid: animation_count rows, frame_count[anim] columns.
// A frame's source rect within the sheet is therefore just
// (frame_index*frame_w, anim_index*frame_h, frame_w, frame_h). Mirrors
// sprite_origin_at's indexing/bounds checks exactly, since origin and
// frame rect must always refer to the same frame.
static Rectangle sprite_frame_rect_at(struct SpriteData *spr, int anim_index, int frame_index){
	Rectangle zero = {0, 0, 0, 0};
	if(!spr || !spr->frame_count || spr->frame_w <= 0 || spr->frame_h <= 0){return zero;}
	if(anim_index < 0 || anim_index >= spr->animation_count){
		LOG(INDEX, "Sprite %d: dir/anim index %d out of range (has %d rows) - falling back to whole texture", spr->gindex, anim_index, spr->animation_count);
		return zero;
	}
	if(frame_index < 0 || frame_index >= spr->frame_count[anim_index]){return zero;}
	return (Rectangle){
		.x = (float)(frame_index * spr->frame_w),
		.y = (float)(anim_index * spr->frame_h),
		.width  = (float)spr->frame_w,
		.height = (float)spr->frame_h,
	};
}
// Same direction -> real_frame column math as entity_sprite_origin_at, so
// this always points at the same frame that entity_sprite_origin_at's
// pivot was computed for.
static Rectangle entity_sprite_frame_rect_at(struct SpriteData *spr, int anim_index, int direction, int frame_index){
	Rectangle zero = {0, 0, 0, 0};
	if(!spr || !spr->frame_count){return zero;}
	if(anim_index < 0 || anim_index >= spr->animation_count){return zero;}
	if(direction < 0 || direction >= DIR_COUNT){return zero;}

	int frames_per_dir = spr->frame_count[anim_index] / DIR_COUNT;
	if(frames_per_dir <= 0){return zero;}
	if(frame_index < 0 || frame_index >= frames_per_dir){return zero;}

	int real_frame = direction * frames_per_dir + frame_index;
	return sprite_frame_rect_at(spr, anim_index, real_frame);
}
static struct DrawItem *create_draw_list(struct EntityManager *entity_manager, struct Map *map, struct SpriteManager *sprite_manager, int *out_count){
	if(!entity_manager || !map || !sprite_manager || !out_count){return NULL;}

	int max_items = map->sprite_count + map->entity_count;
	int item_count = max_items;
		
	for(int i=0; i<map->sprite_count; i++){
		struct Sprite *s = &map->sprite[i];
		if(s->type == GROUND){item_count--;}
		if(s->type == CEILING){item_count--;}
		// GROUND and CEILING are drawn as flat, unsorted layers (see draw_flat_sprites) -
		// only WALL and PROP need depth sorting against each other and against entities.
	}
	struct DrawItem *items = XCALLOC((size_t)item_count, sizeof(struct DrawItem));
	int item_cursor = 0;

	for(int i=0; i<map->sprite_count; i++){
		struct Sprite *s = &map->sprite[i];
		struct SpriteData *spr = sprite_manager->sprite[s->sprite_gindex];
		if(!spr){LOG(IS_NULL, "Sprite %d isn't loaded", s->sprite_gindex);continue;} // not loaded yet - skip rather than draw garbage
		if(s->type == GROUND){continue;}
		if(s->type == CEILING){continue;}
		
		v2 tile = {s->tx, s->ty};	
		vf2 pos = tile_to_world(tile);
		// Map decorations have a single animation (row 0) whose frames are
		// grouped by direction, same layout as entities - s->dir picks the
		// direction-group, and there's only ever frame 0 within that group.
		v2 origin = entity_sprite_origin_at(spr, 0, s->dir, 0);
		Rectangle source = entity_sprite_frame_rect_at(spr, 0, s->dir, 0);
		items[item_cursor].texture  = spr->texture;
		items[item_cursor].source   = source;
		items[item_cursor].position = (Vector2){pos.x - origin.x, pos.y - origin.y};
		items[item_cursor].depth    = pos.y ;
		item_cursor++;
	}

	for(int i=0; i<map->entity_count; i++){
		struct Entity *e = &map->entity[i];
		struct EntityImmutable *imm = entity_manager->immutable_entity[e->prototype_gindex];
		struct EntityMutable *mut   = entity_manager->mutable_entity[e->instance_gindex];
		if(!imm || !mut){continue;}

		struct SpriteData *spr = sprite_manager->sprite[imm->sprite_gindex];
		if(!spr){LOG(IS_NULL, "Sprite %d isn't loaded", imm->sprite_gindex);continue;}

		// Animation row comes from whichever hook is currently active on
		// this entity, resolved through its script - not cached on
		// EntityMutable, since active_hook is the single source of truth
		// and this is cheap to derive. Falls back to row 0 if the entity
		// has no script or the hook can't be resolved (see resolve_hook).
		struct Hook *h = resolve_hook(imm->script, mut->active_hook);
		int anim_index = h ? h->animation : 0;
		
		v2 tile = {mut->position.x, mut->position.y};
		vf2 pos = tile_to_world(tile);

		vf2 offset = {0.0f, 0.0f};
		get_move_offset(mut->mutable_gindex, &offset); // no-op if not moving; offset stays {0,0}
		pos.x += offset.x;
		pos.y += offset.y;
		
		v2 origin = entity_sprite_origin_at(spr, anim_index, mut->direction, mut->current_frame);
		Rectangle source = entity_sprite_frame_rect_at(spr, anim_index, mut->direction, mut->current_frame);

		items[item_cursor].texture  = spr->texture;
		items[item_cursor].source   = source;
		items[item_cursor].position = (Vector2){pos.x - origin.x, pos.y - origin.y};
		items[item_cursor].depth    = pos.y;
		item_cursor++;
	}

	qsort(items, (size_t)item_cursor, sizeof(struct DrawItem), compare_draw_items);
	*out_count = item_cursor;
	return items;
}

// GROUND and CEILING sprites are always the back-most/front-most layer
// respectively - they never need depth comparison against anything else,
// so they're drawn directly in map order instead of going through the
// sort-by-depth list that WALL/PROP/entities use.
static void draw_flat_sprites(struct Map *map, struct SpriteManager *sprite_manager, enum Type type){
	if(!map || !sprite_manager){return;}
	for(int i=0; i<map->sprite_count; i++){
		struct Sprite *s = &map->sprite[i];
		if(s->type != type){continue;}

		struct SpriteData *spr = sprite_manager->sprite[s->sprite_gindex];
		if(!spr){LOG(IS_NULL, "Sprite %d isn't loaded", s->sprite_gindex);continue;}

		v2 tile = {s->tx, s->ty};
		vf2 pos = tile_to_world(tile);
		// Same layout as create_draw_list: s->dir picks the direction-group
		// within animation 0's row, frame 0 within that group.
		v2 origin = entity_sprite_origin_at(spr, 0, s->dir, 0);
		Rectangle source = entity_sprite_frame_rect_at(spr, 0, s->dir, 0);
		if(source.width <= 0 || source.height <= 0){
			source = (Rectangle){0, 0, (float)spr->texture.width, (float)spr->texture.height};
		}
		Vector2 position = {pos.x - origin.x, pos.y - origin.y};
		DrawTextureRec(spr->texture, source, position, WHITE);
	}
}

void draw_map(struct EntityManager *entity_manager, struct Map *map, struct SpriteManager *sprite_manager){
	// The draw item list shouldn't be owned by this function - it needs to be
	// passed in, with some way to detect that only a few entries changed and
	// reorganize just those slots instead of rebuilding + resorting everything
	// every frame. Either way, an XCALLOC every frame is a no-go long term.
	if(!entity_manager || !map || !sprite_manager){return;}

	draw_flat_sprites(map, sprite_manager, GROUND);

	int count = 0;
	struct DrawItem *items = create_draw_list(entity_manager, map, sprite_manager, &count);
	if(items){
		for(int i=0; i<count; i++){
			Rectangle src = items[i].source;
			// frame_w/h weren't set on this sprite's config (or something else
			// went out of bounds) - fall back to the whole texture rather than
			// drawing a zero-size rect (i.e. nothing).
			if(src.width <= 0 || src.height <= 0){
				src = (Rectangle){0, 0, (float)items[i].texture.width, (float)items[i].texture.height};
			}
			DrawTextureRec(items[i].texture, src, items[i].position, WHITE);
		}
		free(items);
	}

	draw_flat_sprites(map, sprite_manager, CEILING);
}
static int compare_draw_items(const void *a, const void *b){
	struct DrawItem *ax = (struct DrawItem*)a;
	struct DrawItem *by = (struct DrawItem*)b;
	if(ax->depth< by->depth){return -1;}
	if(ax->depth> by->depth){return 1;}
	return 0;
}
