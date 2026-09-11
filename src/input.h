#pragma once
#include <raylib.h>
#include <stdbool.h>
#include "util/util.h"

#define MAX_KEYS 5

#define KEY_TYPE_LIST\
	X(KEYBOARD)\
	X(MOUSE)\
	X(GAMEPAD)
#define ACTION_LIST\
	X(LEFT)\
	X(RIGHT)\
	X(UP)\
	X(DOWN)\
	X(ACTION)\
	X(PAN)
enum KeyType{
    	#define X(name) name,
	KEY_TYPE_LIST
	#undef X
	TYPE_COUNT,
};

enum Action{
	#define X(name) name,
	ACTION_LIST
	#undef X
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

vf2 input_vector();

void free_input();

bool pressed(enum Action action);
bool held(enum Action action);
bool released(enum Action action);
