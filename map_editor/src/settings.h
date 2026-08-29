#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "window.h"

#define WINDOW_OPTIONS \
	X(FULLSCREEN, set_fullscreen)\
	X(BORDERLESS, set_borderless)
enum WindowOptions{
	#define X(id, fnc) id,
	WINDOW_OPTIONS
	#undef X
	WINDOW_OPTION_COUNT
};
#define SETTINGS_FLAGS \
	X(IS_COOL)
enum SettingsFlags{
	#define X(id) id,
	SETTINGS_FLAGS
	#undef X
	SETTINGS_FLAG_COUNT
};
#define SETTINGS_STRINGS \
	X(WINDOW_NAME)\
	X(LOG_PATH)
enum SettingsStrings{
	#define X(id) id,
	SETTINGS_STRINGS
	#undef X
	SETTINGS_STRINGS_COUNT
};
#define SETTINGS_INTEGERS \
	X(NUMBER_OF_APPLES)
enum SettingsIntegers{
	#define X(id) id,
	SETTINGS_INTEGERS
	#undef X
	SETTINGS_INTEGER_COUNT
};
#define SETTINGS_FLOATS \
	X(SECONDS_PER_FRAME)
enum SettingsFloats{
	#define X(id) id,
	SETTINGS_FLOATS
	#undef X
	SETTINGS_FLOAT_COUNT
};
struct Settings{
	enum WindowOptions window_option;

	char *strings[SETTINGS_STRINGS_COUNT];
	int integers[SETTINGS_INTEGER_COUNT];
	float floats[SETTINGS_FLOAT_COUNT];
	uint32_t flags;
};

void free_settings();
void load_settings();
void apply_settings();

char *STR(enum SettingsStrings str);
int INT(enum SettingsIntegers in);
float FLT(enum SettingsFloats flt);
bool FLG(enum SettingsFlags flg);

static inline const char *settings_flag_str(enum SettingsFlags id){
	switch(id){
		#define X(id) case id: return #id;
		SETTINGS_FLAGS
		#undef X
		default: return NULL;
	}
}

static inline const char *settings_string_str(enum SettingsStrings id){
	switch(id){
		#define X(id) case id: return #id;
		SETTINGS_STRINGS
		#undef X
		default: return NULL;
	}
}

static inline const char *settings_integer_str(enum SettingsIntegers id){
	switch(id){
		#define X(id) case id: return #id;
		SETTINGS_INTEGERS
		#undef X
		default: return NULL;
	}
}

static inline const char *settings_float_str(enum SettingsFloats id){
	switch(id){
		#define X(id) case id: return #id;
		SETTINGS_FLOATS
		#undef X
		default: return NULL;
	}
}
