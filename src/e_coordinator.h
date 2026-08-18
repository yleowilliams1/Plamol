#pragma once

#include "e_dou_manager.h"
#include "si_flags.h"
#include "si_input.h"
#include "si_map.h"

struct Coordinator{
	struct DouManager *dous;
	struct FlagManager *flag_manager;
	struct InputManager *input;
	struct MapPack *loaded_map;
};
