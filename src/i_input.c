#include <stdlib.h>
#include <string.h>	
#include <raylib.h>
#include "e_engine_settings.h"
#include "i_input.h"
#include "t_config_tool.h"
#include "t_strings.h"
#include "e_error_handler.h"
#include "t_gindex_tool.h"
#include "w_window_manager.h"
#define INVALID_ENUM 10000
static struct KeySet input[A_COUNT] = {0};
void i_set_binding(enum KeyType type, enum Action action, int key);

/*Todo. This is fucntionaly but if you have a out of bounds key setup. Then it may 
 * do weird shit. A fix for later. Also check multiple game pads. Right now it defaults to 
 * 0, check all available ones when you have time to write it.*/
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
bool i_input_pressed(enum Action action) {
    if(!input[action].is_bound){ return false; }
    i_update_input(action);
    return input[action].is_pressed;
}

bool i_input_held(enum Action action) {
    if(!input[action].is_bound){ return false; }
    i_update_input(action); 
    return input[action].is_held;
}

bool i_input_released(enum Action action) {
    if(!input[action].is_bound){ return false; }
     i_update_input(action);
    return input[action].is_released;
}

float i_input_get_mouse_x() {
    return wcnfx(GetMouseX());
}

float i_input_get_mouse_y() {
    return wcnfy(GetMouseY());
}

static void input_parser(struct config_pack p, void *ptr){
	struct parser_set *set = (struct parser_set *)ptr;
	
	int bind_indx = NULL_INDX;
	if(sscanf(p.current_section, "bind.%d", &bind_indx) != 1){
		return;
	}
	if(bind_indx < 0 || bind_indx >= (MAX_KEYS * A_COUNT)){
		ERR_LOG(ERR_PARSE, "To many binds");
		return;
	}

	struct parser_set *active_set = &set[bind_indx];
	active_set->bind = bind_indx;

	if(t_check(p.key, "Action")){
		if(active_set->action != INVALID_ENUM){return;}
		enum Action action = ac_lokup(p.value);
		if(action >= A_COUNT || action < 0 || action == INVALID_ENUM){
			ERR_LOG(ERR_PARSE, "Invalid Action value %s", p.value);
		} else {
			active_set->action = action;
		}
	}
	else if(t_check(p.key, "Input_Type")){
		if(active_set->type != INVALID_ENUM){return;}
		enum KeyType type = ty_lokup(p.value);	
		if(type >= TYPE_COUNT || type < 0 || type == INVALID_ENUM){
			ERR_LOG(ERR_PARSE, "Invalid Type value %s", p.value);
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

void i_init_input(){
	char *path = e_grab_str(INPUT_PATH);
	/*This cannot be NULL. It's impossible. If you
	 * really want to know why I've written a comment in the flag manager
	 * which explains*/
	int set_count = MAX_KEYS * A_COUNT;
	struct parser_set set[set_count];
	for(int i = 0; i < set_count; i++){
		set[i].bind = -1;	
		set[i].action = INVALID_ENUM;
		set[i].type = INVALID_ENUM;
		set[i].key = -1;
	}
	t_config(set, path, input_parser);
	// t_config crashes when it fails so don't check.	
	// commit the actual parser_set to memory
	for(int j = 0; j < set_count; j++){
		struct parser_set *s = &set[j];
		if(s->bind == -1){continue;}
		if(s->action == INVALID_ENUM || s->action < 0 || s->action >= A_COUNT){
			ERR_LOG(ERR_PARSE, "bind.%d missing/invalid Action", j);
			continue;
		}
		if(s->type == INVALID_ENUM){
			ERR_LOG(ERR_PARSE, "bind.%d missing type", j);
			continue;
		}
		if(s->key == -1){
			ERR_LOG(ERR_PARSE, "bind.%d missing Key", j);
			continue;
		}
		i_set_binding(s->type, s->action, s->key);
	}
	ERR_LOG(ERR_OK, "Loaded input");
}
void i_update_input(enum Action action) {
    struct KeySet* key_set = &input[action];
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
            raylib_key_enum = input_key->gamepad_key;   
            key_set->is_pressed  |= IsGamepadButtonPressed(0, raylib_key_enum);
            key_set->is_held     |= IsGamepadButtonDown(0, raylib_key_enum);
            key_set->is_released |= IsGamepadButtonReleased(0, raylib_key_enum);
        }
	  if(input_key->type == MOUSE) {
	      raylib_key_enum = input_key->mouse_key;
	      key_set->is_pressed  |= IsMouseButtonPressed(raylib_key_enum);
	      key_set->is_held     |= IsMouseButtonDown(raylib_key_enum);
	      key_set->is_released |= IsMouseButtonReleased(raylib_key_enum);
	  }
    }
}

void i_set_binding(enum KeyType type, enum Action action, int key){
	input[action].is_bound = true;
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
		if(!input[action].keys[i].is_bound){
			input[action].keys[i] = k;
			return;
		}
	}
	/* All slots full — shift out the oldest and append the new key */
	ERR_LOG(ERR_PARSE, "Put more than %d keys in, overiding the oldest and appending the newst key. This is a config bug.", MAX_KEYS);
	for(int i = 0; i < MAX_KEYS - 1; i++){
		input[action].keys[i] = input[action].keys[i + 1];
	}
	input[action].keys[MAX_KEYS - 1] = k;
}

