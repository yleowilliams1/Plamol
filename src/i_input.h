#pragma once

#include <raylib.h>
#include <stdbool.h>
#include "t_math.h"

#define MAX_KEYS 4

enum KeyType{
    KEYBOARD,
    MOUSE,
    GAMEPAD,
    TYPE_COUNT,
};

enum Action{
    A_WALK_LEFT,
    A_WALK_RIGHT,
    A_WALK_UP,
    A_WALK_DOWN,
    A_ACTION,
    A_PAN,
    A_COUNT,
};

struct InputKey{
    enum KeyType type;
    bool is_bound;
    union{
        KeyboardKey keyboard_key;
        MouseButton mouse_key;
        GamepadButton gamepad_key;
    };
};
struct KeySet{
    struct InputKey keys[MAX_KEYS];
    
    bool is_bound;
    bool is_pressed;
    bool is_held;
    bool is_released;
};

struct parser_set{
	int bind;
	enum Action action;
	enum KeyType type;
	int key;
};

vf2 i_get_input_vector();

bool i_input_pressed(enum Action action);
bool i_input_held(enum Action action);
bool i_input_released(enum Action action);

float i_input_get_mouse_x();
float i_input_get_mouse_y();

void i_update_input(enum Action action);

void i_init_input();
