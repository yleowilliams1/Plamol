#include <stdio.h>

#include "dou_proto_interactable.h"

#include "c_dou_struct_defs.h"
#include "c_flag_enums.h"

#include "e_prototype_manager.h"

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


