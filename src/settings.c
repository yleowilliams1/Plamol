#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "settings.h"
#include "util/util.h"

static void init_val();
static struct Settings *s = NULL;

void load_settings(char *path){
	if(s){free_settings();}
	if(!path){LOG(IS_NULL, "Can't load settings, passed NULL path");return;}
	s = XCALLOC(1, sizeof(struct Settings));
	init_val();

	lua_State *L = luaL_newstate();
	if(!L){LOG(IS_NULL, "Can't create Lua state");return;}
	if(!valua(L, luaL_dofile(L, path))){lua_close(L);
}

	// Load settings table
	lua_getglobal(L, "settings");
	if(!lua_istable(L, -1)){LOG(IS_NULL, "'settings' is missing or not a table in %s", path);lua_close(L); return;}
	for(int i = 0; i < SETTINGS_STRINGS_COUNT; i++){
		const char *name = settings_string_str(i);
		lua_getfield(L, -1, name);
		int t = lua_type(L, -1);
		if(t == LUA_TSTRING){
			if(s->strings[i]){free(s->strings[i]);}
			s->strings[i] = xstrdup(lua_tostring(L, -1));
			LOG(LOAD, "Loaded %s as \"%s\"", name, s->strings[i]);
		}else if(t == LUA_TNIL){
			LOG(IS_NULL, "String %s has not been set!", name);
		}else{
			LOG(IS_NULL, "String %s is a %s, expected string", name, lua_typename(L, t));
		}
		lua_pop(L, 1);
	}

	for(int i = 0; i < SETTINGS_INTEGER_COUNT; i++){
		const char *name = settings_integer_str(i);
		lua_getfield(L, -1, name);
		int t = lua_type(L, -1);
		if(t == LUA_TNUMBER){
			lua_Number n = lua_tonumber(L, -1);
			// range check first so the (int) cast is safe, then reject fractions like 3.5
			if(n >= INT_MIN && n <= INT_MAX && (lua_Number)(int)n == n){
				s->integers[i] = (int)n;
				LOG(LOAD, "Loaded %s as %d", name, s->integers[i]);
			}else{
				LOG(IS_NULL, "Integer %s = %g is not a valid int", name, (double)n);
			}
		}else if(t == LUA_TNIL){
			LOG(IS_NULL, "Integer %s has not been set!", name);
		}else{
			LOG(IS_NULL, "Integer %s is a %s, expected number", name, lua_typename(L, t));
		}
		lua_pop(L, 1);
	}

	for(int i = 0; i < SETTINGS_FLOAT_COUNT; i++){
		const char *name = settings_float_str(i);
		lua_getfield(L, -1, name);
		int t = lua_type(L, -1);
		if(t == LUA_TNUMBER){
			s->floats[i] = (float)lua_tonumber(L, -1);
			LOG(LOAD, "Loaded %s as %f", name, s->floats[i]);
		}else if(t == LUA_TNIL){
			LOG(IS_NULL, "Float %s has not been set!", name);
		}else{
			LOG(IS_NULL, "Float %s is a %s, expected number", name, lua_typename(L, t));
		}
		lua_pop(L, 1);
	}

	for(int i = 0; i < SETTINGS_FLAG_COUNT; i++){
		const char *name = settings_flag_str(i);
		lua_getfield(L, -1, name);
		int t = lua_type(L, -1);
		int value = 0;
		if(t == LUA_TBOOLEAN){value = lua_toboolean(L, -1);}
		else if(t == LUA_TNUMBER){value = lua_tonumber(L, -1) > 0;}
		else if(t != LUA_TNIL){LOG(IS_NULL, "Flag %s is a %s, expected boolean or number", name, lua_typename(L, t));}
		if(value > 0){
			s->flags |= (1 << i);
			LOG(LOAD, "Set %s to true", name);
		}
		lua_pop(L, 1);
	}
	LOG(LOAD, "Loaded settings from %s", path);

	lua_close(L);
}
void free_settings(){
	if(!s){return;}
	for(int i = 0; i < SETTINGS_STRINGS_COUNT; i++){
		if(!s->strings[i]){continue;}
		free(s->strings[i]);
		s->strings[i] = NULL;
	}	
	free(s);
	s = NULL;
	LOG(FREE, "Freed settings");
}
char *STR(enum SettingsStrings str){if(!s){LOG(IS_NULL, "Settings is NULL");return NULL;}return s->strings[str];}
int INT(enum SettingsIntegers in){if(!s){LOG(IS_NULL, "Settings is NULL");return -1;}return s->integers[in];}
float FLT(enum SettingsFloats flt){if(!s){LOG(IS_NULL, "Settings is NULL");return -1;}return s->floats[flt];}
bool FLG(enum SettingsFlags flg) {if(!s){LOG(IS_NULL, "Settings is NULL");return false;}return is_bit(s->flags ,flg);}
static void init_val(){
	if(!s){LOG(IS_NULL, "Cannot initialize settings values while s is NULL");return;}

	char  **str = s->strings;
	int    *ing = s->integers;
	float  *flt = s->floats;

	str[WINDOW_NAME]       = xstrdup("Default Window Name");
	str[LOG_PATH]          = xstrdup("log.txt");
	str[SPRITE_PATH]       = xstrdup("sprites/");
	str[SAVE_PATH]         = xstrdup("saves/");
	str[ENTITY_PATH]       = xstrdup("entity/");
	str[ITEM_PATH]         = xstrdup("item/");
	str[MAP_PATH]          = xstrdup("maps/");
	str[INPUT_CONFIG_PATH] = xstrdup("input.lua");

	ing[SPRITE_COUNT]            = 0;
	ing[ENTITY_INSTANCE_COUNT]   = 0;
	ing[ITEM_COUNT]              = 0;
	ing[ENTITY_IMMUTABLE_COUNT]  = 0;

	flt[SECONDS_PER_FRAME] = 0.4f;
	flt[CAM_MIN_ZOOM]      = 0.5f;
	flt[CAM_MAX_ZOOM]      = 6.0f;
	flt[CAM_FACTOR]        = 1.1f;
	flt[SECONDS_PER_TILE]  = 0.3f;
	flt[FLOAT_TIME]        = 2.0f;
}
