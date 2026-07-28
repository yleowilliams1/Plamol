#pragma once
#include <stdint.h>
#include <stdbool.h>

#define MAX_SEGS 256 

enum SegmentFlags{
	F_PASSABLE,
	F_HURT,
	F_WATER,
	F_DIRT,
	SEGFLAG_COUNT,
};

enum SegmentProperties{
	SEG_TILEGINDX,
	SEG_TILETEXTURE_GINDX,
	SEG_START_X,
	SEG_START_Y,
	SEG_END_X,
	SEG_END_Y,
	SEG_FLAG,
	SEGPROP_COUNT,
};

enum MetadataProperties{
	M_TILESET,
	M_SEGMENT_CAP,
	M_NORTH_EXIT,
	M_SOUTH_EXIT,
	M_WEST_EXIT,
	M_EAST_EXIT,
	M_META_COUNT,
};

struct MapSegment{
	int data[SEGPROP_COUNT]; 
};
struct MapData{
	int meta[M_META_COUNT];
	struct MapSegment seg[MAX_SEGS];
};

bool m_free_map(int gindx);
bool m_load_map(int gindx);
bool m_get_metadata(int gindx, bool autoload, enum MetadataProperties m, int *out);
bool m_get_seg(int gindx, bool autoload, enum SegmentProperties m, int segment, int *out);
