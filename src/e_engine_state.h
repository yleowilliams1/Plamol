#pragma once
#include <stdbool.h>

#include "si_flags.h"
#include "si_input.h"
#include "si_stage.h"

#include "t_depot_manager.h"

struct EngineState{
	struct DepotManager *depot_manager;

	struct FlagManager *flag_manager;
	struct InputManager *input_manager;
	
	struct Stage *stage;
};

struct EngineState *e_create_engine_state();
bool e_free_engine_state(struct EngineState *engine_state);

bool e_update_engine_state(struct EngineState *engine_state);
bool e_draw_engine_state(struct EngineState *engine_state);
