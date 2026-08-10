#include <stdio.h>
#include <raylib.h>
#include "l_sprite_manager.h"
#include "m_render_map.h"
#include "m_map.h"

#define Z_HEIGHT 100

Vector2 tile_to_world(int tile_x, int tile_y, int tile_z, int tile_width, int tile_height, int z_height){
	Vector2 world;
	world.x = (float)(tile_x - tile_y) * (tile_width / 2.0f);
	world.y = (float)(tile_x + tile_y) * (tile_height / 2.0f);
	world.y -= (float)(tile_z * z_height);
	return world;
}
static void draw_floor_tile(struct MapSegmentData *seg, int tx, int ty){
	if(seg->floor_gindx < 0){return;}
	Vector2 scale = l_grab_sprite_scale(seg->floor_gindx, true);
	l_draw_sprite(seg->floor_gindx, true, tile_to_world(tx, ty, seg->z, scale.x, scale.y, Z_HEIGHT));
}
static void draw_wall_tile(struct MapSegmentData *seg, int tx, int ty){
	if(seg->wall_gindx < 0){return;}
	Vector2 scale = l_grab_sprite_scale(seg->wall_gindx, true);
	l_draw_sprite(seg->wall_gindx, true, tile_to_world(tx, ty, seg->z, scale.x, scale.y, Z_HEIGHT));
}

static void draw_segment(struct MapSegmentData *seg, int player_z){
	if((seg->flags & (1 << INVISIBLE))){return;}
	if((seg->flags & (1 << HIDE_IF_ABOVE_PLAYER)) && seg->z > player_z){return;}

	int x0 = seg->start_tile.x, x1 = seg->end_tile.x;
	int y0 = seg->start_tile.y, y1 = seg->end_tile.y;
	if(x1 < x0){int t = x0; x0 = x1; x1 = t;}
	if(y1 < y0){int t = y0; y0 = y1; y1 = t;}

	if(seg->flags & (1 << HAS_FLOORS_REC)){
		for(int ty = y0; ty <= y1; ty++)
			for(int tx = x0; tx <= x1; tx++)
				draw_floor_tile(seg, tx, ty);
	}

	if(seg->flags & (1 << IS_WALLS)){ // fully solid block
		for(int ty = y0; ty <= y1; ty++)
			for(int tx = x0; tx <= x1; tx++)
				draw_wall_tile(seg, tx, ty);
		return;
	}

	if(seg->flags & (1 << HAS_WALLS_REC)){ // hollow perimeter
		for(int tx = x0; tx <= x1; tx++){draw_wall_tile(seg, tx, y0); draw_wall_tile(seg, tx, y1);}
		for(int ty = y0; ty <= y1; ty++){draw_wall_tile(seg, x0, ty); draw_wall_tile(seg, x1, ty);}
	}

	if(seg->flags & (1 << WALL_IS_NORTH)) for(int tx = x0; tx <= x1; tx++) draw_wall_tile(seg, tx, y0);
	if(seg->flags & (1 << WALL_IS_SOUTH)) for(int tx = x0; tx <= x1; tx++) draw_wall_tile(seg, tx, y1);
	if(seg->flags & (1 << WALL_IS_WEST))  for(int ty = y0; ty <= y1; ty++) draw_wall_tile(seg, x0, ty);
	if(seg->flags & (1 << WALL_IS_EAST))  for(int ty = y0; ty <= y1; ty++) draw_wall_tile(seg, x1, ty);

printf("seg z=%d flags=0x%X floor_gindx=%d wall_gindx=%d visible=%d has_floors=%d has_walls_rec=%d\n",
	seg->z, seg->flags, seg->floor_gindx, seg->wall_gindx,
	!!(seg->flags & (1 << INVISIBLE)),
	!!(seg->flags & (1 << HAS_FLOORS_REC)),
	!!(seg->flags & (1 << HAS_WALLS_REC)));

}

static void draw_interactable(struct MapInteractableData *ia, int player_z){
	if(ia->z > player_z){return;} // same visibility rule as HIDE_IF_ABOVE_PLAYER; no per-interactable flag exists yet
	Vector2 scale = l_grab_sprite_scale(ia->gindx, true);
	l_draw_sprite(ia->gindx, true, tile_to_world(ia->tile_position.x, ia->tile_position.y, ia->z, scale.x, scale.y, Z_HEIGHT));
}

void m_draw_map(struct MapPack *m, int player_z){
	if(!m){return;}
	for(int i = 0; i < m->m.meta[M_SEGMENT_COUNT]; i++){
		draw_segment(&m->d.s[i], player_z);
	}
	for(int i = 0; i < m->m.meta[M_INTERACTABLE_COUNT]; i++){
		draw_interactable(&m->d.i[i], player_z);
	}
}
