#pragma once
#include <stdint.h>

#define INTERACTABLE_TYPE \
	X(INTERACTABLE_DOOR) 
#define INTERACTABLE_FLAG \
	X(INTERACTABLE_IS_OPEN)

enum InteractableType{
	#define X(name) name,
	INTERACTABLE_TYPE
	#undef X
	INTERACTABLE_TYPE_COUNT,
};
enum InteractableFlags{
	#define X(name) name,
	INTERACTABLE_FLAG
	#undef X
	INTERACTABLE_FLAG_COUNT,
};

struct InteractablePrototypeInteractData{
};
struct InteractableInstanceInteractData{
};

struct InteractablePrototype{
	int sprite_gindx;
	uint32_t flags;
};
struct InteractableInstance{
};

struct ItemFunctions;
struct ItemFunctions interactable_prototype();
struct ItemFunctions interactable_instance();
