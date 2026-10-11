#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "flag.h"

struct PacketReference{
	// The file name is saves/save_uid-save_index.SAVE
	uint32_t save_uid;
	uint32_t save_index;
};
struct SaveFile{
	struct PacketReference *refs;
	uint32_t ref_count;
	uint32_t current_map_gindex;
	struct FlagManager *fm;
};
struct SaveManager{
	struct SaveFile *saves;
	int32_t save_count;
};
