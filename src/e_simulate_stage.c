#include "t_log_handler.h"

#include "e_simulate_stage.h"

#include "si_input.h"
#include "si_stage.h"

void e_simulate_stage(struct InputManager *input_manager, struct Stage *stage){
	if(!input_manager){LOG(LOG_NULL, "Input manager is NULL"); return;}	
	if(!stage){LOG(LOG_NULL, "Stage is NULL");return;}
	
	
}
