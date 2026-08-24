#pragma once

#include "t_depot_manager.h"

#include "depo_entity.h"
#include "depo_interactable.h"
#include "depo_proto_items.h"
#include "depo_sprite.h"

#define DEPOT_LIST \
	X(DPO_ITEM_PROTO,   struct ItemPrototype,         item_prototype,         struct ItemPrototypeInteractData) \
	X(DPO_SPRITE,       struct SpriteData,            sprite_data,            struct SpriteDataInteractData) \
	X(DPO_INTER_PROTO,  struct InteractablePrototype, interactable_prototype, struct InteractablePrototypeInteractData) \
	X(DPO_INTER_INST,   struct InteractableInstance,  interactable_instance,  struct InteractableInstanceInteractData) \
	X(DPO_ENTITY_PROTO, struct EntityPrototype,       entity_prototype,       struct EntityPrototypeInteractData) \
	X(DPO_ENTITY_INST,  struct EntityInstance,        entity_instance,        struct EntityInstanceInteractData)

enum DepotType{
	#define X(id, type, fncs, itype) id,
	DEPOT_LIST
	#undef X
	DEPOT_COUNT
};
#define X(id, type, fncs, itype) \
static inline void load_depot_##id(struct DepotManager *dman){ \
	t_load_depot(dman, id, fncs()); \
} \
static inline void free_depot_##id(struct DepotManager *dman){ \
	t_free_depot(dman, id); \
} \
static inline type *load_##id(struct DepotManager *dman, struct LoadData ld, int item_index){ \
	return (type *)t_load_item(dman, id, ld, item_index, sizeof(type)); \
} \
static inline type *grab_##id(struct DepotManager *dman, int item_index){ \
	return (type *)t_grab_item(dman, id, item_index); \
} \
static inline void free_##id(struct DepotManager *dman, int item_index){ \
	t_free_item(dman, id, item_index); \
} \
static inline void interact_##id(struct DepotManager *dman, itype *interactdata, int item_index){ \
	t_on_interact(dman, (void *)interactdata, id, item_index); \
}
DEPOT_LIST
#undef X
const char *depstr(enum DepotType depot){
	switch(depot){
		#define X(id, type, fncs, itype) case id: return #id;
		DEPOT_LIST
		#undef X
		case DEPOT_COUNT: return NULL;
		default: return NULL;
	}
}
