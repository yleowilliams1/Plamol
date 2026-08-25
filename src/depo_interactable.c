#include <stdio.h>

#include "t_depot_manager.h"
#include "depo_interactable.h"

static void prototype_on_free(void *slot);
static void prototype_on_init(void *slot);
static void prototype_on_pload(void *slot);
static void prototype_on_load(struct config_pack p, void *ptr);
static void prototype_on_interact(void *interactadata, void *slot);

static void instance_on_free(void *slot);
static void instance_on_init(void *slot);
static void instance_on_pload(void *slot);
static void instance_on_load(struct config_pack p, void *ptr);
static void instance_on_bulk(void *loadata, void *ptr);
static void instance_on_interact(void *interactadata, void *slot);

struct ItemFunctions instance_interactable(){
	return (struct ItemFunctions){
		.on_load = instance_on_load,
		.on_init = instance_on_init,
		.on_free = instance_on_free,
		.on_pload = instance_on_pload,
		.on_bulk = instance_on_bulk,
		.on_interact= instance_on_interact,
	};
}
struct ItemFunctions prototype_interactable(){
	return (struct ItemFunctions){
		.on_load = prototype_on_load,
		.on_init = prototype_on_init,
		.on_free = prototype_on_free,
		.on_pload = prototype_on_pload,
		.on_bulk = NULL,
		.on_interact = prototype_on_interact,
	};
}


static void prototype_on_free(void *slot){
}
static void prototype_on_init(void *slot){
}
static void prototype_on_pload(void *slot){
}
static void prototype_on_load(struct config_pack p, void *ptr){
}
static void prototype_on_interact(void *interactadata, void *slot){
}

static void instance_on_free(void *slot){
}
static void instance_on_init(void *slot){
}
static void instance_on_pload(void *slot){
}
static void instance_on_load(struct config_pack p, void *ptr){
}
static void instance_on_bulk(void *loadata, void *ptr){
}
static void instance_on_interact(void *interactadata, void *slot){
}
