#include <stdlib.h>
#include <string.h>	
#include <raylib.h>
#include "e_engine_settings.h"
#include "si_input.h"
#include "t_config_tool.h"
#include "t_strings.h"
#include "t_log_handler.h"
#include "t_gindex_tool.h"
#include "w_window_manager.h"
#include "t_math.h"
#include "c_magic_number.h"

static void input_parser(struct config_pack p, void *ptr);
static void si_set_binding(enum KeyType type, enum Action action, int key, struct InputManager *input);
static void si_update_input(enum Action action, struct InputManager *input);

static enum Action ac_lokup(char *str){
	if(t_check(str, "walk_left")){return A_WALK_LEFT;}
	if(t_check(str, "walk_right")){return A_WALK_RIGHT;}
	if(t_check(str, "walk_up")){return A_WALK_UP;}
	if(t_check(str, "walk_down")){return A_WALK_DOWN;}
	if(t_check(str, "action")){return A_ACTION;}
	if(t_check(str, "pan")){return A_PAN;}
	return INVALID_ENUM;	
}
static enum KeyType ty_lokup(char *str){
	if(t_check(str, "Keyboard")){return KEYBOARD;}
	if(t_check(str, "Mouse")){return MOUSE;}
	if(t_check(str, "Gamepad")){return GAMEPAD;}
	return INVALID_ENUM;
}
bool si_pressed(enum Action action, struct InputManager *input) {
    	if(!input->input[action].is_bound){ return false; }
    	si_update_input(action, input);
    	return input->input[action].is_pressed;
}

bool si_held(enum Action action, struct InputManager *input) {
    	if(!input->input[action].is_bound){ return false; }
    	si_update_input(action, input);
    	return input->input[action].is_held;
}

bool si_released(enum Action action, struct InputManager *input) {
    	if(!input->input[action].is_bound){ return false; }
    	si_update_input(action, input);
    	return input->input[action].is_released;
}

vf2 si_input_vector(struct InputManager *input){
	vf2 v = {0};
	const float leftStickDeadzoneX = 0.1f;
	const float leftStickDeadzoneY = 0.1f;
	
	// Check joystick input manually
	// first then check non-joystick
	for(int i = 0; i < NUMBER_OF_GAMEPADS_TO_CHECK; i++){
		if(IsGamepadAvailable(i)){
			float leftStickX = GetGamepadAxisMovement(i, GAMEPAD_AXIS_LEFT_X);
			float leftStickY = GetGamepadAxisMovement(i, GAMEPAD_AXIS_LEFT_Y);	
			if (leftStickX > -leftStickDeadzoneX && leftStickX < leftStickDeadzoneX) leftStickX = 0.0f;
			if (leftStickY > -leftStickDeadzoneY && leftStickY < leftStickDeadzoneY) leftStickY = 0.0f;
		
			v.x = leftStickX;
			v.y = leftStickY;		
		}
	}
	if(si_held(A_WALK_UP, input)){v.y = 1;};
	if(si_held(A_WALK_DOWN, input)){v.y = -1;}
	if(si_held(A_WALK_RIGHT, input)){v.x = 1;}
	if(si_held(A_WALK_LEFT, input)){v.x = -1;}

	return v;
}

struct InputManager *si_init_input(){
	char *path = e_grab_sipath(SI_INPUT);
	if(!path){LOG(LOG_NULL, "e_grab_sipath returned NULL"); return NULL;}
	
	int set_count = MAX_KEYS * A_COUNT;
	struct parser_set set[set_count];
	for(int i = 0; i < set_count; i++){
		set[i].bind = -1;	
		set[i].action = INVALID_ENUM;
		set[i].type = INVALID_ENUM;
		set[i].key = -1;
	}
	struct InputManager *input = XCALLOC(1, sizeof(struct InputManager));
	
	bool configured = t_config(set, path, input_parser);
	if(!configured){LOG(LOG_NULL, "Failed to configure"); return NULL;}
	
	for(int j = 0; j < set_count; j++){
		struct parser_set *s = &set[j];
		if(s->bind == -1){continue;}
		if(s->action == INVALID_ENUM || s->action < 0 || s->action >= A_COUNT){
			LOG(LOG_PARSE, "bind.%d missing/invalid Action", j);
			continue;
		}
		if(s->type == INVALID_ENUM){
			LOG(LOG_PARSE, "bind.%d missing type", j);
			continue;
		}
		if(s->key == -1){
			LOG(LOG_PARSE, "bind.%d missing Key", j);
			continue;
		}
		si_set_binding(s->type, s->action, s->key, input);
	}
	LOG(LOG_LOAD, "Loaded input");
	return input;
}
static void si_update_input(enum Action action, struct InputManager *input) {
    	struct KeySet* key_set = &input->input[action];
    	if(!key_set->is_bound){ return; }

    	key_set->is_pressed  = false;
    	key_set->is_held     = false;
    	key_set->is_released = false;

    	for(int key = 0; key < MAX_KEYS; key++) {
        	struct InputKey* input_key = &key_set->keys[key];
        	if(!input_key->is_bound){continue;}
        	int raylib_key_enum;

        	if(input_key->type == KEYBOARD) {
            		raylib_key_enum = input_key->keyboard_key;  
            		key_set->is_pressed  |= IsKeyPressed(raylib_key_enum);
            		key_set->is_held     |= IsKeyDown(raylib_key_enum);
            		key_set->is_released |= IsKeyReleased(raylib_key_enum);
        	}
        	if(input_key->type == GAMEPAD) {
			for(int i = 0; i < NUMBER_OF_GAMEPADS_TO_CHECK; i++){	
				raylib_key_enum = input_key->gamepad_key;   
				key_set->is_pressed  |= IsGamepadButtonPressed(i, raylib_key_enum);
				key_set->is_held     |= IsGamepadButtonDown(i, raylib_key_enum);
				key_set->is_released |= IsGamepadButtonReleased(i, raylib_key_enum);
			}
        	}
	  	if(input_key->type == MOUSE) {
	      		raylib_key_enum = input_key->mouse_key;
	      		key_set->is_pressed  |= IsMouseButtonPressed(raylib_key_enum);
	      		key_set->is_held     |= IsMouseButtonDown(raylib_key_enum);
	      		key_set->is_released |= IsMouseButtonReleased(raylib_key_enum);
	  	}
    	}
}

static void si_set_binding(enum KeyType type, enum Action action, int key, struct InputManager *input){
	input->input[action].is_bound = true;
	struct InputKey k = (struct InputKey){
		.type = type,
		.is_bound = true
	};

	switch (type){
		case KEYBOARD:
			k.keyboard_key = key;
			break;
		case MOUSE:
			k.mouse_key = key;
			break;
		case GAMEPAD:
			k.gamepad_key = key;
			break;
		default:
			k.mouse_key = 0;
			break;	
	}
	
	for(int i = 0; i < MAX_KEYS; i++){
		if(!input->input[action].keys[i].is_bound){
			input->input[action].keys[i] = k;
			return;
		}
	}
	/* All slots full — shift out the oldest and append the new key */
	LOG(LOG_PARSE, "Put more than %d keys in, overiding the oldest and appending the newst key. This is a config bug.", MAX_KEYS);
	for(int i = 0; i < MAX_KEYS - 1; i++){
		input->input[action].keys[i] = input->input[action].keys[i + 1];
	}
	input->input[action].keys[MAX_KEYS - 1] = k;
}
static void input_parser(struct config_pack p, void *ptr){
	if(!ptr){LOG(LOG_NULL, "NULL parse_set in parser"); return;}
	struct parser_set *set = (struct parser_set *)ptr;
	
	int bind_indx = NULL_INDX;
	if(sscanf(p.current_section, "bind.%d", &bind_indx) != 1){
		LOG(LOG_PARSE, "Unknown section [%s]", p.current_section);
		return;
	}
	if(bind_indx < 0 || bind_indx >= (MAX_KEYS * A_COUNT)){
		LOG(LOG_PARSE, "Bind indx %d is invalid", bind_indx);
		return;
	}

	struct parser_set *active_set = &set[bind_indx];
	active_set->bind = bind_indx;

	if(t_check(p.key, "Action")){
		if(active_set->action != INVALID_ENUM){return;}
		enum Action action = ac_lokup(p.value);
		if(action >= A_COUNT || action < 0 || action == INVALID_ENUM){
			LOG(LOG_PARSE, "Invalid Action value %s", p.value);
		} else {
			active_set->action = action;
		}
	}
	else if(t_check(p.key, "Input_Type")){
		if(active_set->type != INVALID_ENUM){return;}
		enum KeyType type = ty_lokup(p.value);	
		if(type >= TYPE_COUNT || type < 0 || type == INVALID_ENUM){
			LOG(LOG_PARSE, "Invalid Type value %s", p.value);
		} else{
			active_set->type = type;
		}
	}
	else if(t_check(p.key, "Key")){
		// Check if it's already initalized
		if(active_set->key != -1){return;}
		t_atoi(p.value, &active_set->key);
		
	}
}


