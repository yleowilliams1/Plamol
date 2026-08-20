#include <stdio.h>

#include "dou_proto_interactable.h"

#include "c_dou_struct_defs.h"
#include "c_flag_enums.h"

#include "e_dou_manager.h"

static void interactable_on_free(void *slot);
static void interactable_on_init(void *slot);
static void interactable_on_pload(void *slot);
static void interactable_on_load(struct config_pack p, void *ptr);

struct DouLoader dou_interactable(){
	return (struct DouLoader){
		.func.on_load = interactable_on_load,
		.func.on_init = interactable_on_init,
		.func.on_free = interactable_on_free,
		.func.on_pload = interactable_on_pload,
		.size = sizeof(struct DouInteractablePrototype),
		.type = EOU_INTERACTABLE,
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


