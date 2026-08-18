#pragma once
#include "c_magic_number.h"

#include <raylib.h>
#include <stdbool.h>
#include "t_math.h"

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

struct InputManager{
	struct KeySet input[A_COUNT];
};

vf2 si_input_vector(struct InputManager *input);

bool si_pressed(enum Action action, struct InputManager *input);
bool si_held(enum Action action, struct InputManager *input);
bool si_released(enum Action action, struct InputManager *input);

struct InputManager *si_init_input();
