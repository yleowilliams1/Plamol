#include <stdio.h>
#include "e_dou_manager.h"
#include "dou_proto_interactable.h"
#include "t_config_tool.h"

static void interactable_on_free(void *slot);
static void interactable_on_init(void *slot);
static void interactable_on_pload(void *slot);
static void interactable_on_load(struct config_pack p, void *ptr);

struct DouLoader dou_interactable(){
	return (struct DouLoader){
		.on_load = interactable_on_load,
		.on_init = interactable_on_init,
		.on_free = interactable_on_free,
		.on_pload = interactable_on_pload,
		.size = sizeof(struct Interactable),
		.type = DOU_INTERACTABLE,
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


