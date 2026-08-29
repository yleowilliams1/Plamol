#include <stdio.h>
#include <stdbool.h>

#include "t_log_handler.h"

#include "e_engine_state.h"
#include "e_simulate_stage.h"

#include "si_flags.h"
#include "si_input.h"
#include "si_stage.h"

#define DEFAULT_STAGE 0

struct EngineState *e_create_engine_state(){
	struct EngineState *engine_state = XCALLOC(1, sizeof(struct EngineState));	

	engine_state->flag_manager = si_init_flag();	
	engine_state->input_manager = si_init_input();
	engine_state->stage = si_init_stage(DEFAULT_STAGE);

	return engine_state;
}
bool e_free_engine_state(struct EngineState *engine_state){
	if(!engine_state){return false;}
	
	si_free_flag(engine_state->flag_manager);
	if(engine_state->input_manager){free(engine_state->input_manager);}

	si_free_stage(engine_state->stage);
	engine_state->stage = NULL;

	free(engine_state);
	engine_state = NULL;

	return true;
}
bool e_update_engine_state(struct EngineState *engine_state){
	if(!engine_state){return false;}
	e_simulate_stage(engine_state->input_manager, engine_state->stage);
	return true;
}
bool e_draw_engine_state(struct EngineState *engine_state){
	if(!engine_state){return false;}
	si_draw_stage(engine_state->stage);
	return true;
}
