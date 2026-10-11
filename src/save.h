#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "flag.h"
#include "entity.h"

struct SaveFile{
	struct SaveComponenet *objs;
	uint32_t obj_count;
	uint32_t current_map_gindex;
	struct FlagManager *fm;
};
struct SaveManager{
	struct SaveFile *saves;
	int32_t save_count;
};
