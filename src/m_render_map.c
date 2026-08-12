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

static void draw_floor(const struct MapDecompTile *tile, v2 tpos);
static void draw_props(const struct MapDecompTile *tile, v2 tpos, int player_z);
static bool fcheck(uint32_t flag, enum TileFlags type);
static Vector2 m_tile_to_world(int tile_x, int tile_y, int tile_z);
void m_draw_map(struct MapPack *m, int player_z, struct EntityInstance *pool, int ent_size){
	if(!m || !m->d.t){return;}
	int w = m->m.meta[M_WIDTH];
	int h = m->m.meta[M_HEIGHT];

	// TWO PASSES. Floors and walls cannot be interleaved per tile: a wall
	// sits on the FAR edge of its tile, so the next tile's floor is drawn
	// after it and paints over its base. That is what turns a solid north
	// or east wall into a row of floating pillars with gaps between them.
	//
	// Within each pass, iterate by depth (x + y) rather than row-major.
	// Row-major happens to be a valid painter's order while every tile
	// shares one z, and stops being one the moment they don't.
	for(int depth = 0; depth <= (w - 1) + (h - 1); depth++){
		for(int y = 0; y < h; y++){
			int x = depth - y;
			if(x < 0 || x >= w){continue;}
			v2 indx = {x, y};
			draw_floor(&m->d.t[y * w + x], indx);
		}
	}
	for(int depth = 0; depth <= (w - 1) + (h - 1); depth++){
		for(int y = 0; y < h; y++){
			int x = depth - y;
			if(x < 0 || x >= w){continue;}
			v2 indx = {x, y};
			draw_props(&m->d.t[y * w + x], indx, player_z);
		}
	}
	if(!pool){ERR_LOG(ERR_NULL, "Entity pool empty");return;}
	for(int i = 0; i < ent_size; i++){
		if(!pool[i].valid){continue;}	
	}
}

// Centre of the tile, in world space.
static Vector2 m_tile_to_world(int tile_x, int tile_y, int tile_z){
	Vector2 world;
	world.x = (float)(tile_x - tile_y) * (TILE_X / 2.0f);
	world.y = (float)(tile_x + tile_y) * (TILE_Y / 2.0f);
	world.y -= (float)(tile_z * Z_HEIGHT);
	return world;
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
	if(fcheck(tile->flags, T_HAS_WALL)){
		if(tile->wall_gindx < 0){
			ERR_LOG(ERR_PARSE, "Tile %d.%d has T_HAS_WALL but wall_gindx is %d", tpos.x, tpos.y, tile->wall_gindx);
		} else if(!(fcheck(tile->flags, T_HIDE_IF_ABOVE) && tile->wall_z > player_z)){
			l_draw_sprite(tile->wall_gindx, true, m_tile_to_world(tpos.x, tpos.y, tile->wall_z), 0, tile->dir);
		}
	}
}

static bool fcheck(uint32_t flag, enum TileFlags type){
	return flag & (1u << type);
}
