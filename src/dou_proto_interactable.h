#pragma once
#include <stdint.h>

enum InteractableFlags{
	INT_IS_OPEN,	
};
enum InteractableType{
	INT_DOOR,
	INT_CONTAINER,
};
enum InteractableComponenets{
	INT_SPRITE,
	INT_INVENTORY_GINDX,
	INT_COM_COUNT,
};
struct Interactable{
	enum InteractableType type;
	uint32_t flags;
	int data[INT_COM_COUNT];
};

struct DouLoader;

struct DouLoader dou_interactable();
