#pragma once

#include <raylib.h>
#include <stdbool.h>

#define MAX_KEYS 4

enum KeyType{
    KEYBOARD,
    MOUSE,
    GAMEPAD,
};

enum Action{
    A_WALK_LEFT,
    A_WALK_RIGHT,
    A_WALK_UP,
    A_WALK_DOWN,
    A_ACTION,
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

bool i_input_pressed(enum Action action);
bool i_input_held(enum Action action);
bool i_input_released(enum Action action);

float i_input_get_mouse_x();
float i_input_get_mouse_y();

void i_update_input(enum Action action);

void i_set_binding(enum KeyType type, enum Action action, int key);
void i_default_binds();
