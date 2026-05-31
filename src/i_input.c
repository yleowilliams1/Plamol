#include <raylib.h>
#include "i_input.h"

static struct KeySet input[A_COUNT] = {0};

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
    return GetMouseX();
}

float i_input_get_mouse_y() {
    return GetMouseY();
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
	for(int i = 0; i < MAX_KEYS - 1; i++){
		input[action].keys[i] = input[action].keys[i + 1];
	}
	input[action].keys[MAX_KEYS - 1] = k;
}

void i_default_binds() {
}
