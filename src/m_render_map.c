#include <stdio.h>
#include <raylib.h>
#include <stdint.h>
#include "l_sprite_manager.h"
#include "m_render_map.h"
#include "m_map.h"
#include "e_error_handler.h"
#define Z_HEIGHT 100

#define TILE_X 64
#define TILE_Y 32

static Vector2 tile_to_world(int tile_x, int tile_y, int tile_z);
static void draw_tile(const struct MapDecompTile *tile, v2 tpos);
static bool tile_checker(const struct MapDecompTile *tile);
static bool fcheck(uint32_t flag, enum TileFlags type);

void m_draw_map(struct MapPack *m, int player_z){
	if(!m){return;}
	int size = m->m.meta[M_WIDTH] * m->m.meta[M_HEIGHT];
	for(int i = 0; i < size; i++){
		v2 indx =  {i % m->m.meta[M_WIDTH], i / m->m.meta[M_WIDTH]};
		draw_tile(&m->d.t[i], indx);		
	}
}
// This will give a centered position
static Vector2 tile_to_world(int tile_x, int tile_y, int tile_z){
	Vector2 world;
	world.x = (float)(tile_x - tile_y) * (TILE_X/ 2.0f);
	world.y = (float)(tile_x + tile_y) * (TILE_Y/ 2.0f);
	world.y += TILE_Y/ 2.0f;
	world.y -= (float)(tile_z * Z_HEIGHT);
	return world;
}
static void draw_tile(const struct MapDecompTile *tile, v2 tpos){
	if(!tile_checker(tile)){ERR_LOG(ERR_NULL, "tile_checker failed, skipping tile at x:%d.y:%d", tpos.x, tpos.y); return;}
	if(fcheck(tile->flags, T_HAS_TILE)){
		Vector2 world_pos = tile_to_world(tpos.x, tpos.y, tile->floor_z);
		l_draw_sprite(tile->floor_gindx, true, world_pos, 0, 0);	
	}
	// Animation 0 is corners and 1 is straights
	if(fcheck(tile->flags, T_HAS_WALL)){
		Vector2 world_pos = tile_to_world(tpos.x, tpos.y, tile->wall_z);
		bool animation = !fcheck(tile->flags, T_IS_CORNER);
		l_draw_sprite(tile->wall_gindx, true, world_pos, (int)animation, tile->dir);
	}	
}
static bool tile_checker(const struct MapDecompTile *tile){
	if(fcheck(tile->flags, T_INVISIBLE)){return false;}
	if((fcheck(tile->flags, T_HAS_WALL) || fcheck(tile->flags, T_IS_CORNER) || fcheck(tile->flags, T_WALL_COLLIDE) || fcheck(tile->flags, T_MERGE_WALL)) && tile->wall_gindx < 0){
		ERR_LOG(ERR_PARSE, "Tile has a wall flag checked, but wall_gindx is set to invalid %d", tile->wall_gindx);
		return false;
	}
	if((fcheck(tile->flags, T_HAS_TILE) || fcheck(tile->flags, T_FLOOR_COLLIDE) || fcheck(tile->flags, T_MERGE_FLOOR)) && tile->floor_gindx < 0){
		ERR_LOG(ERR_PARSE, "Tile has T_HAS_TILE flag checked, but floor_gindx is set to invalid %d", tile->floor_gindx);
		return false; 
	}
	if(fcheck(tile->flags, T_HAS_INTERACTABLE) && tile->interactable_gindx < 0){
		ERR_LOG(ERR_PARSE, "Tile has T_HAS_INTERACTABLE flag checked, but interactable_gindx is set to invalid %d", tile->interactable_gindx);
		return false;
	}
	return true;
}
static bool fcheck(uint32_t flag, enum TileFlags type){
	return flag & (1 << type);
}
