#pragma once

#include "t_depot_manager.h"

#include "depo_entity.h"
#include "depo_interactable.h"
#include "depo_proto_items.h"
#include "depo_sprite.h"

#define DEPOT_LIST \
	X(DPO_ITEM_PROTO,   struct ItemPrototype,         prototype_item) \
	X(DPO_SPRITE,       struct SpriteData,            dou_sprite) \
	X(DPO_INTER_PROTO,  struct InteractablePrototype, prototype_interactable) \
	X(DPO_ENTITY_PROTO, struct EntityPrototype,       entity_prototype) \

enum DepotType{
	#define X(id, type, fncs) id,
	DEPOT_LIST
	#undef X
	DEPOT_COUNT
};
#define X(id, type, fncs) \
static inline void load_depot_##id(struct DepotManager *dman){ \
	t_load_depot(dman, id, fncs()); \
} \
static inline void free_depot_##id(struct DepotManager *dman){ \
	t_free_depot(dman, id); \
} \
static inline type *load_##id(struct DepotManager *dman, int item_index){ \
	return (type *)t_load_item(dman, id, item_index, sizeof(type)); \
} \
static inline type *grab_##id(struct DepotManager *dman, int item_index){ \
	return (type *)t_grab_item(dman, id, item_index, sizeof(type)); \
} \
static inline void free_##id(struct DepotManager *dman, int item_index){ \
	t_free_item(dman, id, item_index); \
} 
DEPOT_LIST
#undef X
const char *depstr(enum DepotType depot);
