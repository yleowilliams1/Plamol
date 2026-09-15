#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

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
	X(LOG_PATH)\
	X(SPRITE_PATH)\
	X(SAVE_PATH)\
	X(ENTITY_PATH)\
	X(ITEM_PATH)\
	X(MAP_PATH)\
	X(INPUT_CONFIG_PATH)
enum SettingsStrings{
	#define X(id) id,
	SETTINGS_STRINGS
	#undef X
	SETTINGS_STRINGS_COUNT
};
#define SETTINGS_INTEGERS \
	X(SPRITE_COUNT)\
	X(ENTITY_INSTANCE_COUNT)\
	X(ITEM_COUNT)\
	X(ENTITY_IMMUTABLE_COUNT)
enum SettingsIntegers{
	#define X(id) id,
	SETTINGS_INTEGERS
	#undef X
	SETTINGS_INTEGER_COUNT
};
#define SETTINGS_FLOATS \
	X(SECONDS_PER_FRAME)\
	X(CAM_MIN_ZOOM)\
	X(CAM_MAX_ZOOM)\
	X(CAM_FACTOR)\
	X(SECONDS_PER_TILE)
enum SettingsFloats{
	#define X(id) id,
	SETTINGS_FLOATS
	#undef X
	SETTINGS_FLOAT_COUNT
};
struct Settings{
	char *strings[SETTINGS_STRINGS_COUNT];
	int integers[SETTINGS_INTEGER_COUNT];
	float floats[SETTINGS_FLOAT_COUNT];
	uint32_t flags;
};

void free_settings();
void load_settings(char *path);

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
