#include <stdio.h>

#include "t_depot_manager.h"
#include "t_instance_manager.h"
#include "depo_interactable.h"

static void prototype_on_free(void *slot);
static void prototype_on_init(void *slot);
static void prototype_on_pload(void *slot);
static void prototype_on_load(struct config_pack p, void *ptr);

static void instance_on_serialize(void *slot, FILE *file);
static void instance_on_deserialize(void *slot, FILE *file);
static void instance_on_free(void *slot);

struct InstanceFunctions instance_interactable(){
	return (struct InstanceFunctions){
		.on_serialize = instance_on_serialize,
		.on_deserialize = instance_on_deserialize,
		.on_free = instance_on_free,	
	};
}
struct ItemFunctions prototype_interactable(){
	return (struct ItemFunctions){
		.on_load = prototype_on_load,
		.on_init = prototype_on_init,
		.on_free = prototype_on_free,
		.on_pload = prototype_on_pload,
	};
}

static void instance_on_serialize(void *slot, FILE *file){
}
static void instance_on_deserialize(void *slot, FILE *file){
}
static void instance_on_free(void *slot){
}



static void prototype_on_free(void *slot){
}
static void prototype_on_init(void *slot){
}
static void prototype_on_pload(void *slot){
}
static void prototype_on_load(struct config_pack p, void *ptr){
}
