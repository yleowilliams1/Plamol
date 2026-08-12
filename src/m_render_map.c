#include <stdio.h>
#include <raylib.h>
#include <stdint.h>
#include "l_sprite_manager.h"
#include "m_render_map.h"
#include "m_map.h"
#include "e_error_handler.h"
#include "p_entity_instance.h"
// One z level must equal the on-screen rise of one wall block or stacked
// levels won't sit flush. Measured against data/sprites/1.png: a floor drawn
// at z+1 lands exactly on the wall tops at 56, not 100.
#define Z_HEIGHT 56

#define TILE_X 64
#define TILE_Y 32

// Entities drawn per frame. The pool is 256; anything past this is dropped
// with a warning rather than smashing the stack.
#define MAX_DRAW_ENTITIES 256

// One entity queued for drawing, keyed by the same depth the tiles sort on.
struct EntityDraw{
	int depth;
	int pool_indx;
};

static void draw_floor(const struct MapDecompTile *tile, v2 tpos);
static void draw_props(const struct MapDecompTile *tile, v2 tpos, int player_z);
static void draw_entity(struct EntityInstance *e);
static int world_to_depth(int world_y);
static int collect_entities(struct EntityInstance *pool, int ent_size, struct EntityDraw *out, int max_depth);
static bool fcheck(uint32_t flag, enum TileFlags type);
static Vector2 m_tile_to_world(int tile_x, int tile_y, int tile_z);

void m_draw_map(struct MapPack *m, int player_z, struct EntityInstance *pool, int ent_size){
	if(!m || !m->d.t){return;}
	int w = m->m.meta[M_WIDTH];
	int h = m->m.meta[M_HEIGHT];
	int max_depth = (w - 1) + (h - 1);

	// TWO PASSES. Floors and walls cannot be interleaved per tile: a wall
	// sits on the FAR edge of its tile, so the next tile's floor is drawn
	// after it and paints over its base. That is what turns a solid north
	// or east wall into a row of floating pillars with gaps between them.
	//
	// Within each pass, iterate by depth (x + y) rather than row-major.
	// Row-major happens to be a valid painter's order while every tile
	// shares one z, and stops being one the moment they don't.
	for(int depth = 0; depth <= max_depth; depth++){
		for(int y = 0; y < h; y++){
			int x = depth - y;
			if(x < 0 || x >= w){continue;}
			v2 indx = {x, y};
			draw_floor(&m->d.t[y * w + x], indx);
		}
	}

	// Entities sort into the SAME depth order as the tiles, so the props
	// pass can merge the two. An entity standing on tile (x,y) is drawn
	// after that tile's own wall (it stands in front of it) and before any
	// tile in the next band (which is nearer the camera and must overlap
	// it). Both sequences are ordered by depth, so this is a linear merge
	// with one cursor -- no per-tile search through the pool.
	struct EntityDraw ents[MAX_DRAW_ENTITIES];
	int ent_count = collect_entities(pool, ent_size, ents, max_depth);
	int ecursor = 0;

	for(int depth = 0; depth <= max_depth; depth++){
		for(int y = 0; y < h; y++){
			int x = depth - y;
			if(x < 0 || x >= w){continue;}
			v2 indx = {x, y};
			draw_props(&m->d.t[y * w + x], indx, player_z);
		}
		while(ecursor < ent_count && ents[ecursor].depth == depth){
			draw_entity(&pool[ents[ecursor].pool_indx]);
			ecursor++;
		}
	}
	// Anything that sorted past the last band still gets drawn rather than
	// silently vanishing.
	while(ecursor < ent_count){
		draw_entity(&pool[ents[ecursor].pool_indx]);
		ecursor++;
	}
}

// Gathers the drawable entities and sorts them by depth. Returns the count.
// Insertion sort: the list is tiny and almost always already ordered, so this
// is O(n) in practice. If the pool ever gets big, swap it for a counting sort
// keyed on depth -- the range is 0..max_depth, so it's a natural fit.
static int collect_entities(struct EntityInstance *pool, int ent_size, struct EntityDraw *out, int max_depth){
	if(!pool){ERR_LOG(ERR_NULL, "No entity pool passed to the map renderer"); return 0;}
	int count = 0;
	for(int i = 0; i < ent_size; i++){
		if(!pool[i].valid){continue;}
		if(!(pool[i].e.flags & (1u << ENT_SPRITE))){continue;}
		if(count >= MAX_DRAW_ENTITIES){
			ERR_LOG(ERR_OUTOFBOUNDS, "More than %d drawable entities, dropping the rest", MAX_DRAW_ENTITIES);
			break;
		}
		// E_POSX/E_POSY are WORLD pixels, not tile indices, so the depth key
		// has to be recovered from the projection. Only world.y depends on
		// (tile_x + tile_y): it is that sum times TILE_Y/2, so dividing by
		// TILE_Y/2 gives the band back. world.x can't help -- it encodes
		// (tile_x - tile_y), which is constant along a depth band.
		int depth = world_to_depth(pool[i].e.data[E_POSY]);
		// Off-map entities still draw; clamping only decides which band
		// they merge into, never where they land on screen.
		if(depth < 0){depth = 0;}
		if(depth > max_depth){depth = max_depth;}

		int j = count - 1;
		while(j >= 0 && out[j].depth > depth){
			out[j + 1] = out[j];
			j--;
		}
		out[j + 1] = (struct EntityDraw){.depth = depth, .pool_indx = i};
		count++;
	}
	return count;
}

static void draw_entity(struct EntityInstance *e){
	if(!e){return;}
	if(e->e.data[E_SPRITE] < 0){
		ERR_LOG(ERR_PARSE, "Entity GUID %d has ENT_SPRITE but sprite gindx is %d", e->GUID, e->e.data[E_SPRITE]);
		return;
	}
	// Already world space -- no projection, and no rounding, since the
	// position is integer pixels by design.
	Vector2 pos = {(float)e->e.data[E_POSX], (float)e->e.data[E_POSY]};
	l_draw_sprite(e->e.data[E_SPRITE], true, pos, 0, e->e.data[E_DIRECTION]);
}

// Centre of the tile, in world space.
static Vector2 m_tile_to_world(int tile_x, int tile_y, int tile_z){
	Vector2 world;
	world.x = (float)(tile_x - tile_y) * (TILE_X / 2.0f);
	world.y = (float)(tile_x + tile_y) * (TILE_Y / 2.0f);
	world.y -= (float)(tile_z * Z_HEIGHT);
	return world;
}

// Inverse of the world.y half of the projection: which depth band a world
// pixel row belongs to. Floors toward negative rather than truncating toward
// zero, so entities north or west of the origin land in the right band
// instead of jumping one forward across y = 0.
static int world_to_depth(int world_y){
	int band = TILE_Y / 2;
	if(world_y >= 0){return world_y / band;}
	return -(((-world_y) + band - 1) / band);
}

static void draw_floor(const struct MapDecompTile *tile, v2 tpos){
	if(!tile){return;}
	if(fcheck(tile->flags, T_INVISIBLE)){return;}
	if(!fcheck(tile->flags, T_HAS_TILE)){return;}
	if(tile->floor_gindx < 0){
		ERR_LOG(ERR_PARSE, "Tile %d.%d has T_HAS_TILE but floor_gindx is %d", tpos.x, tpos.y, tile->floor_gindx);
		return;
	}
	l_draw_sprite(tile->floor_gindx, true, m_tile_to_world(tpos.x, tpos.y, tile->floor_z), 0, 0);
}

static void draw_props(const struct MapDecompTile *tile, v2 tpos, int player_z){
	if(!tile){return;}
	if(fcheck(tile->flags, T_INVISIBLE)){return;}
	if(fcheck(tile->flags, T_HAS_INTERACTABLE)){
		if(tile->interactable_gindx < 0){
			ERR_LOG(ERR_PARSE, "Tile %d.%d has T_HAS_INTERACTABLE but interactable_gindx is %d", tpos.x, tpos.y, tile->interactable_gindx);
		} else {
			l_draw_sprite(tile->interactable_gindx, true, m_tile_to_world(tpos.x, tpos.y, tile->interactable_z), 0, tile->dir);
			// Don't draw the wall
			return;
		}
	}
	// Each element validates itself now. Previously one bad gindx anywhere
	// on the tile threw away the whole tile, floor included.
	if(fcheck(tile->flags, T_HAS_WALL)){
		if(tile->wall_gindx < 0){
			ERR_LOG(ERR_PARSE, "Tile %d.%d has T_HAS_WALL but wall_gindx is %d", tpos.x, tpos.y, tile->wall_gindx);
		} else if(!(fcheck(tile->flags, T_HIDE_IF_ABOVE) && tile->wall_z > player_z)){
			// One row of 8 frames, indexed directly by enum TileDirections.
			l_draw_sprite(tile->wall_gindx, true, m_tile_to_world(tpos.x, tpos.y, tile->wall_z), 0, tile->dir);
		}
	}
}

static bool fcheck(uint32_t flag, enum TileFlags type){
	return flag & (1u << type);
}
