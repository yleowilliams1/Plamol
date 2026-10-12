#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "flag.h"
#include "entity.h"
struct ObjectRef{
	uint32_t uid;
	uint32_t slot;
};
struct SaveFile{
	struct ObjectRef *objs;
	uint32_t obj_count;
	uint32_t current_map_gindex;
	struct FlagManager *fm;
};
struct SaveManager{
	struct SaveFile *saves;
	int32_t save_count;
	uint32_t last_loaded_save;
};
