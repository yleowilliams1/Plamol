#include <stdio.h>

#include "t_register.h"

static void interactable_on_free(void *slot);
static void interactable_on_init(void *slot);
static void interactable_on_pload(void *slot);
static void interactable_on_load(struct config_pack p, void *ptr);

struct ItemFunctions prototype_interactable(){
	return (struct ItemFunctions){
		.on_load = interactable_on_load,
		.on_init = interactable_on_init,
		.on_free = interactable_on_free,
		.on_pload = interactable_on_pload,
		.on_bulk = NULL,
	};
}

static void interactable_on_free(void *slot){
}
static void interactable_on_init(void *slot){
}
static void interactable_on_pload(void *slot){
}
static void interactable_on_load(struct config_pack p, void *ptr){
}


