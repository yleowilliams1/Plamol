#pragma once
#include <stdint.h>
#include "c_stat_list.h"

#define MODIFIER_LIST\
	X(ADD_MOD)\
	X(HIT_MOD)\
	X(DAMAGE_MOD)\
	X(CONSUME_MOD)

#define ITEM_STRING_LIST \
	X(ITEM_NAME) \
	X(ITEM_DESCRIPTION) 
#define ITEM_FLAGS \
	X(IS_THROWABLE) \
	X(IS_CONSUMEABLE) 

enum ItemFlags{
	#define X(name) name,
	ITEM_FLAGS
	#undef X
	ITEM_FLAG_COUNT,
};
enum ModifierType{
	#define X(name) name,
	MODIFIER_LIST
	#undef X
	MODIFIER_COUNT,
};
enum ItemStringType{
	#define X(name) name,
	ITEM_STRING_LIST
	#undef X
	ITEM_STRING_COUNT,
};
struct ItemStatModifier{
	enum StatType type;
	int stat;	
	int amount;
};
struct ItemPrototypeInteractData{

};
struct ItemPrototype{
	struct ItemStatModifier mods[MODIFIER_COUNT];	
	char *strings[ITEM_STRING_COUNT];
	int range;
	uint32_t flags;
};

struct ItemFunctions;
struct ItemFunctions item_prototype();

const char *modstr(enum ModifierType type){
	switch(type){
		#define X(name) case name: return #name;
		MODIFIER_LIST
		#undef X
		default: return NULL;
	}
}
const char *itemstrstr(enum ItemStringType type){
	switch(type){
		#define X(name) case name: return #name;
		ITEM_STRING_LIST	
		#undef X
		default: return NULL;
	}
}
const char *itemflgstr(enum ItemFlags flag){
	switch(flag){
		#define X(name) case name: return #name;
		ITEM_FLAGS	
		#undef X
		default: return NULL;
	}	
}
