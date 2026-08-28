#pragma once
#include <stdio.h>

#define INSTANCE_LIST \
	X(INST_ENTITY) \
	X(INST_INTERACTABLE)
enum InstanceType{
	#define X(id) id,
	INSTANCE_LIST
	#undef X
	INSTANCE_COUNT
};
const char *inststr(enum InstanceType type){
	switch(type){
		#define X(id) case id: return #id;
		INSTANCE_LIST
		#undef X
		case INSTANCE_COUNT: return NULL;
		default: return NULL;
	}
}
