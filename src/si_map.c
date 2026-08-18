#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "si_map.h"
#include "t_config_tool.h"
#include "e_engine_settings.h"
#include "t_log_handler.h"
#include "t_strings.h"

static void metadata_parser(struct config_pack p, void *ptr);
static void data_parser(struct config_pack p, void *ptr);
static bool m_decompress_segments(struct MapParsePackage *pckg);

static const char *metadata_lokup[M_META_COUNT] = {
	[M_NORTH_EXIT] = "north_map",
	[M_SOUTH_EXIT] = "south_map",
	[M_WEST_EXIT] = "west_map",
	[M_EAST_EXIT] = "east_map",
	[M_ENTITY_COUNT] = "entity_count",
	[M_INTERACTABLE_COUNT] = "interactable_count",
	[M_SEGMENT_COUNT] = "segment_count",
	[M_WIDTH] = "width",
	[M_HEIGHT] = "height",
};
static const char *segment_flag_lokup[SEGMENT_FLAGS_COUNT] = {
	[INVISIBLE] = "invisible",
	[IS_WALLS] = "is_wall",
	[WALL_IS_NORTH] = "wall_is_north",
	[WALL_IS_SOUTH] = "wall_is_south",
	[WALL_IS_EAST] = "wall_is_east",
	[WALL_IS_WEST] = "wall_is_west",
	[HAS_WALLS_REC] = "has_walls_rec",
	[HAS_FLOORS_REC] = "has_floors_rec",
	[IS_WALLS_COLLIDE] = "is_walls_collide",
	[IS_FLOOR_COLLIDE] = "is_floor_collide",
	[ALWAYS_ABOVE_PLAYER] = "always_above_player",
	[SHOULD_MERGE_WALL] = "should_merge_wall",
	[SHOULD_MERGE_FLOOR] = "should_merge_floor",
	[HIDE_IF_ABOVE_PLAYER] = "hide_if_above_player",
};

bool si_free_map(struct MapPack *map){
	if(!map){LOG(LOG_NULL, "Map is NULL can't free");}
	if(map->entities){free(map->entities);}
	if(map->tiles){free(map->tiles);}	
	free(map);
	map = NULL;
	return true; 
}

struct MapPack *si_load_map(int gindx){
	struct MapPack *map = XCALLOC(1, sizeof(struct MapPack));
	char *base = e_grab_sipath(SI_MAP);
	if(!base){LOG(LOG_NULL, "e_grab_sipath returned NULL"); return NULL;}
	
	char *path = t_format_path(base, ".ini", gindx);

	bool metadata_parsed = t_config(map->metadata, path ,metadata_parser);
	if(!metadata_parsed){LOG(LOG_ABORT, "Failed to load map[%s] metadata", path);return false;}
	
	int *meta = map->metadata;
	map->entities = XCALLOC(0 , meta[M_ENTITY_COUNT] * sizeof(struct MapEntityData));
	map->tiles = XCALLOC(0, meta[M_WIDTH] * meta[M_HEIGHT] * sizeof(struct MapDecompTile));

	// Now that we have the metadata and the 
	// allocated arrays we can parse the actual
	// text file into memory 
	struct MapParsePackage pckg = {
		.map = map,
		.segments = XCALLOC(0, meta[M_SEGMENT_COUNT] * sizeof(struct MapSegmentData)),
		.interactables = XCALLOC(0, meta[M_INTERACTABLE_COUNT] * sizeof(struct MapInteractableData)),
	};
	
	bool data_parsed = t_config(&pckg, path, data_parser);
	if(!data_parsed){
		LOG(LOG_ABORT, "Failed to parse map[%s] data", e_grab_sipath(SI_MAP)); 
		free(map->entities);
		free(map->tiles);
		free(pckg.segments);
		free(pckg.interactables);
		return false;
	};
	
	// Now we decompress into a 
	// faster heap array of tiles
	bool decompressed = m_decompress_segments(&pckg);	
	if(!decompressed){
		LOG(LOG_ABORT, "Failed to decompress map[%s] data", e_grab_sipath(SI_MAP)); 
		free(map->entities);
		free(map->tiles);
		free(pckg.segments);
		free(pckg.interactables);
		return false;
	};
	
	// Cleanup the useless segments
	free(pckg.segments);
	free(pckg.interactables);
	free(path);
	return map;
}

static void metadata_parser(struct config_pack p, void *ptr){
	int *m = (int *)ptr;	
	if(!m){LOG(LOG_ABORT, "Passed null metadata to parser.");}
	
	if(t_check(p.current_section, "metadata")){
		for(int i = 0; i < M_META_COUNT; i++){
			if(!t_check(p.key, (char *)metadata_lokup[i])){continue;}
			t_atoi(p.value, &m[i]);	
		}
	}
}
static void data_parser(struct config_pack p, void *ptr){
	struct MapParsePackage *m = (struct MapParsePackage*)ptr;	
	if(!m){LOG(LOG_ABORT, "Passed NULL pointer"); return;}	
	int *meta = m->map->metadata;	
	struct MapSegmentData *segs = m->segments;
	struct MapEntityData *entities = m->map->entities;
	struct MapInteractableData *inter = m->interactables;

	if(!meta || !segs || !entities || !inter){LOG(LOG_ABORT, "Passed NULL pointer"); return;}	

	int entity_indx;
	if(sscanf(p.current_section, "entity.%d", &entity_indx) == 1){
		if(entity_indx < 0){LOG(LOG_PARSE, "Tried to create parse entity with a negative index"); return;}	
		if(entity_indx >= meta[M_ENTITY_COUNT]){LOG(LOG_PARSE, "Tried to parse entity with index larger than set entity count %d", meta[M_ENTITY_COUNT]); return;}
		if(t_check(p.key, "entity_gindx")){
			t_atoi(p.value, &entities[entity_indx].gindx);
		} else if(t_check(p.key, "tile_spawn_x")){
			t_atoi(p.value, &entities[entity_indx].tile_spawn_x);
		} else if(t_check(p.key, "tile_spawn_y")){
			t_atoi(p.value, &entities[entity_indx].tile_spawn_y);
		} else if(t_check(p.key, "GUID")){
			t_atoi(p.value, &entities[entity_indx].GUID);
		} else if(t_check(p.key, "Direction")){
			t_atoi(p.value, (int *)&entities[entity_indx].dir);
			if(entities[entity_indx].dir < 0 || entities[entity_indx].dir > W_DIR_COUNT){
				LOG(LOG_PARSE, "Invalid entity direction");
				entities[entity_indx].dir = W_NORTH;
			}
		}	
	}
	int interactable_indx;
	if(sscanf(p.current_section, "interactable.%d", &interactable_indx) == 1){
		if(interactable_indx < 0){LOG(LOG_PARSE, "Tried to parse interactable with a negative index"); return;}	
		if(interactable_indx >= meta[M_INTERACTABLE_COUNT]){LOG(LOG_PARSE, "Tried to parse interactable with index larger than set interactable count %d", meta[M_INTERACTABLE_COUNT]); return;}
		if(t_check(p.key, "z")){
			t_atoi(p.value, &inter[interactable_indx].z);
		} else if(t_check(p.key, "tile_x")){
			t_atoi(p.value, &inter[interactable_indx].tile_position.x);
		} else if(t_check(p.key, "tile_y")){
			t_atoi(p.value, &inter[interactable_indx].tile_position.y);
		} else if(t_check(p.key, "interactable_gindx")){
			t_atoi(p.value, &inter[interactable_indx].gindx);
		}	
	}

	int seg_indx;
	if(sscanf(p.current_section, "segment.%d", &seg_indx) == 1){
		if(seg_indx < 0){LOG(LOG_PARSE, "Tried to parse segment with a negative index"); return;}	
		if(seg_indx >= meta[M_SEGMENT_COUNT]){LOG(LOG_PARSE, "Tried to parse segment with index larger than set segment count %d", meta[M_SEGMENT_COUNT]); return;}
		for(int i = 0; i < SEGMENT_FLAGS_COUNT; i++){
			char *str = (char *)segment_flag_lokup[i];
			if(!t_check(p.key, str)){continue;}
			int value;
			t_atoi(p.value, &value);
			if(value > 0){segs[seg_indx].flags |= (1 << i);}				
		}
		
		if(t_check(p.key, "start_tile_x")){
			t_atoi(p.value, &segs[seg_indx].start_tile.x);
		} else if(t_check(p.key, "start_tile_y")){
			t_atoi(p.value, &segs[seg_indx].start_tile.y);
		} else if(t_check(p.key, "end_tile_x")){
			t_atoi(p.value, &segs[seg_indx].end_tile.x);
		} else if(t_check(p.key, "end_tile_y")){
			t_atoi(p.value, &segs[seg_indx].end_tile.y);
		} else if(t_check(p.key, "z")){
			t_atoi(p.value, &segs[seg_indx].z);
		} else if(t_check(p.key, "wall_gindx")){
			t_atoi(p.value, &segs[seg_indx].wall_gindx);
		} else if(t_check(p.key, "floor_gindx")){
			t_atoi(p.value, &segs[seg_indx].floor_gindx);
		}
	}
}
static bool m_decompress_segments(struct MapParsePackage *pckg){
	if(!pckg) {return false;}
	if(!pckg->map){return false;}

	int width = pckg->map->metadata[M_WIDTH];
	int height = pckg->map->metadata[M_HEIGHT];
	int seg_count = pckg->map->metadata[M_SEGMENT_COUNT];
	int interactable_count = pckg->map->metadata [M_INTERACTABLE_COUNT];

	// 1. Process Segments into Decompressed Tiles
	for (int i = 0; i < seg_count; i++) {
		struct MapSegmentData *seg = &pckg->segments[i];

	    	int min_x = seg->start_tile.x < seg->end_tile.x ? seg->start_tile.x : seg->end_tile.x;
	    	int max_x = seg->start_tile.x > seg->end_tile.x ? seg->start_tile.x : seg->end_tile.x;
	    	int min_y = seg->start_tile.y < seg->end_tile.y ? seg->start_tile.y : seg->end_tile.y;
	    	int max_y = seg->start_tile.y > seg->end_tile.y ? seg->start_tile.y : seg->end_tile.y;

	    	if (min_x < 0) min_x = 0;
	    	if (min_y < 0) min_y = 0;
	    	if (max_x >= width)  max_x = width - 1;
	    	if (max_y >= height) max_y = height - 1;

	    	for (int y = min_y; y <= max_y; y++) {
			for (int x = min_x; x <= max_x; x++) {
		    		int tile_idx = y * width + x;
		    		struct MapDecompTile *tile = &pckg->map->tiles[tile_idx];
				
				bool is_edge_x = (x == min_x || x == max_x);
		    		bool is_edge_y = (y == min_y || y == max_y);
		    		bool is_perimeter = is_edge_x || is_edge_y;
		    		bool is_corner   = is_edge_x && is_edge_y;

		    		// Wall Placement & Direction Resolution
		    		bool place_wall = false;
		    		// Default to a straight, not W_NORTH: W_NORTH is a corner
		    		// frame now, so it's a bad fallback for the IS_WALLS
		    		// no-direction-flags case below.
		    		enum TileDirections dir = W_NORTH_EAST;

		    		if ((seg->flags & (1 << HAS_WALLS_REC)) && is_perimeter) {
					place_wall = true;
					if (is_corner) {
			    			// A corner is defined by BOTH edges that meet
			    			// there, so resolve x AND y together, never as
			    			// an x-or-y chain.
			    			if (x == min_x) dir = (y == min_y) ? W_NORTH : W_EAST;
			    			else            dir = (y == min_y) ? W_WEST  : W_SOUTH;
					}
					else if (y == min_y) dir = W_NORTH_EAST;
					else if (y == max_y) dir = W_SOUTH_WEST;
					else if (x == min_x) dir = W_NORTH_WEST;
					else if (x == max_x) dir = W_SOUTH_EAST;
		    		} else if (seg->flags & (1 << IS_WALLS)) {
					if ((seg->flags & (1 << WALL_IS_NORTH)) && y == min_y) { place_wall = true; dir = W_NORTH_EAST; }
					else if ((seg->flags & (1 << WALL_IS_SOUTH)) && y == max_y) { place_wall = true; dir = W_SOUTH_WEST; }
					else if ((seg->flags & (1 << WALL_IS_EAST)) && x == min_x) { place_wall = true; dir = W_NORTH_WEST; }
					else if ((seg->flags & (1 << WALL_IS_WEST)) && x == max_x) { place_wall = true; dir = W_SOUTH_EAST; }
					else {
			    			uint32_t dir_mask = (1 << WALL_IS_NORTH) | (1 << WALL_IS_SOUTH) |(1 << WALL_IS_EAST)  | (1 << WALL_IS_WEST);
			    			if ((seg->flags & dir_mask) == 0) place_wall = true;
					}
		    		}

		    		if (place_wall) {
					tile->flags |= (1 << T_HAS_WALL);
					tile->wall_gindx = seg->wall_gindx;
					tile->wall_z = seg->z;
					tile->dir = dir;
		    		}
		    		// Floor Placement
		    		if (seg->flags & (1 << HAS_FLOORS_REC)) {
					tile->flags |= (1 << T_HAS_TILE);
					tile->floor_gindx = seg->floor_gindx;
					tile->floor_z = seg->z;
		    		}

		    		// Map Segment Flags to TileFlags
		    		if (seg->flags & (1 << INVISIBLE))           tile->flags |= (1 << T_INVISIBLE);
		    		if (seg->flags & (1 << IS_WALLS_COLLIDE))    tile->flags |= (1 << T_WALL_COLLIDE);
		    		if (seg->flags & (1 << IS_FLOOR_COLLIDE))    tile->flags |= (1 << T_FLOOR_COLLIDE);
		    		if (seg->flags & (1 << ALWAYS_ABOVE_PLAYER)) tile->flags |= (1 << T_ALWAYS_ABOVE);
		    		if (seg->flags & (1 << SHOULD_MERGE_WALL))   tile->flags |= (1 << T_MERGE_WALL);
		    if (seg->flags & (1 << SHOULD_MERGE_FLOOR))  tile->flags |= (1 << T_MERGE_FLOOR);
		    		if (seg->flags & (1 << HIDE_IF_ABOVE_PLAYER)) tile->flags |= (1 << T_HIDE_IF_ABOVE);
		}
	    }
	}

	// 2. Stamp Interactables into Tile Grid
	for (int i = 0; i < interactable_count; i++) {
	    struct MapInteractableData *inter = &pckg->interactables[i];

	    int x = inter->tile_position.x;
	    int y = inter->tile_position.y;

	    if (x >= 0 && x < width && y >= 0 && y < height) {
		int tile_idx = y * width + x;
		struct MapDecompTile *tile = &pckg->map->tiles[tile_idx];

		tile->flags |= (1 << T_HAS_INTERACTABLE);
		tile->interactable_gindx = inter->gindx;
		tile->interactable_z = inter->z;
	    }
	}
	return true;
}
