#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "m_map.h"
#include "t_config_tool.h"
#include "e_engine_settings.h"
#include "e_error_handler.h"
#include "t_strings.h"
#include "t_gindex_tool.h"
#include "l_asset_manager.h"

#define MAP_CAP 16

#define MAX_STRUCTS 512
#define MAX_FUNCTIONS 512

static void map_parser(struct config_pack p, void *ptr);

static struct MapData maps[MAP_CAP] = {0};
static struct local_indx iman[MAP_CAP] = {0};

static struct MapSegment g_pallete[MAX_STRUCTS] = {0};

static const char *metadata_lokup[M_META_COUNT] = {
	[M_TILESET] = "tileset",
	[M_SEGMENT_CAP] = "segment_size",
	[M_NORTH_EXIT] = "north_map",
	[M_SOUTH_EXIT] = "south_map",
	[M_WEST_EXIT] = "west_map",
	[M_EAST_EXIT] = "east_map",
};

static const char *seg_flag_lokup[SEGFLAG_COUNT] = {
	[F_PASSABLE] = "passable",
	[F_HURT] = "hurt",
	[F_WATER] = "water",
	[F_DIRT] = "dirt",

};
bool m_free_map(int gindx){
	struct AssetFreePackage pckg = {
		.gindx = gindx,
		.index_manager = iman,
		.arr_cap = MAP_CAP,
		.arr = maps,
		.element_size = sizeof(struct MapData),		
	};

	return t_free_asset(pckg);
}

bool m_load_map(int gindx){
	struct AssetLoadPackage pckg = {
		.gindx = gindx,
		.index_manager = iman,
		.arr_cap = MAP_CAP,
		.arr = maps,
		.element_size = sizeof(struct MapData),
		.function = map_parser,
		.path = e_grab_str(MAP_PATH),
		.init = NULL,
	};

	return l_load_asset(pckg);
}

bool m_get_metadata(int gindx, bool autoload, enum MetadataProperties m, int *out){
	int lindx = l_getter_checks(gindx, autoload, MAP_CAP, iman, m_load_map);
	if(!t_indxvalid(MAP_CAP, lindx)){ERR_LOG(ERR_NULL, "Failed to find map %d", gindx); return false;}
	
	*out = maps[lindx].meta[m];
	return true;
}
bool m_get_seg(int gindx, bool autoload, enum SegmentProperties m, int segment, int *out){
	int lindx = l_getter_checks(gindx, autoload, MAP_CAP, iman, m_load_map);
	if(!t_indxvalid(MAP_CAP, lindx)){ERR_LOG(ERR_NULL, "Failed to find map %d", gindx); return false;}
	
	*out = maps[lindx].seg[segment].data[m];
	return true;
}

static void map_parser(struct config_pack p, void *ptr){
	
	struct MapData *m = (struct MapData*)ptr;	
	if(!m){
		ERR_LOG(ERR_FUCKED, "Passed null map to parser. Shouldn't be possible");	
	}
	
	if(t_check(p.current_section, "metadata")){
		for(int i = 0; i < M_META_COUNT; i++){
			if(!t_check(p.key, (char *)metadata_lokup[i])){continue;}
			t_atoi(p.value, &m->meta[i]);	
		}
	}
	if(m->meta[M_SEGMENT_CAP] <= 0){ERR_LOG(ERR_FUCKED, "%d is not a valid Segment Cap! Either rewrite of reorder the ini file so that M_SEGMENT_CAP is at the top of the file", m->meta[M_SEGMENT_CAP]);}

	// Scan palletes
	int struct_indx;
	if(sscanf(p.current_section, "palletes.%d", &struct_indx) == 1){
		if(struct_indx < 0 || struct_indx >= m->meta[M_SEGMENT_CAP]){return;}
		
		struct MapSegment *slot = &g_pallete[struct_indx];	
		if(t_check(p.key, "tile_indx")){
			t_atoi(p.value, &slot->data[SEG_TILEGINDX]);
		} else if(t_check(p.key, "tile_texture_index")){
			t_atoi(p.value, &slot->data[SEG_TILETEXTURE_GINDX]);
		}
		// flags
		for(int i = 0; i < SEGFLAG_COUNT; i++){
			if(!t_check(p.key, (char *)seg_flag_lokup[i])){continue;};
			int value;
			t_atoi(p.value, &value);
			if(value < 0){ERR_LOG(ERR_PARSE, "Tried to parse flag %s with value of less than 0", seg_flag_lokup[i]);}
			if(value > 0){slot->data[SEG_FLAG] |= (1 << i);}
		}
		return;	
	}

	// This will access older pallete memory
	// if you access a unset pallet. Thats an issue.
	// Too bad!
	int func_indx;
	if(sscanf(p.current_section, "function.%d", &func_indx) == 1){
		if(func_indx < 0 || func_indx >= m->meta[M_SEGMENT_CAP]){return;}
		struct MapSegment *seg = &m->seg[func_indx];
		if(t_check(p.key, "start_x")){
			t_atoi(p.value, &seg->data[SEG_START_X]);
		} else if(t_check(p.key, "start_y")){
			t_atoi(p.value, &seg->data[SEG_START_Y]);
		} else if(t_check(p.key, "end_x")){
			t_atoi(p.value, &seg->data[SEG_END_X]);
		} else if(t_check(p.key, "end_y")){
			t_atoi(p.value, &seg->data[SEG_END_Y]);
		} else if(t_check(p.key, "pallete")){
			int pallete; 
			t_atoi(p.value, &pallete);
			if(pallete < 0 || pallete >= MAX_STRUCTS){ERR_LOG(ERR_PARSE, "INVALID PALLETE INDEX"); return;}
			seg->data[SEG_TILEGINDX] = g_pallete[pallete].data[SEG_TILEGINDX];
			seg->data[SEG_TILETEXTURE_GINDX]= g_pallete[pallete].data[SEG_TILETEXTURE_GINDX];		
			seg->data[SEG_FLAG] = g_pallete[pallete].data[SEG_FLAG];
		}
		return;
	}	
}
