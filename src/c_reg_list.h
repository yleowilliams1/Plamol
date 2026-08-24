#pragma once

#include "t_register.h"

#include "reg_entity.h"
#include "reg_proto_interactable.h"
#include "reg_proto_items.h"
#include "reg_sprite_manager.h"

#define REGISTER_LIST \
	X(REG_ITEM,         struct ItemPrototype,         item_prototype) \
	X(REG_SPRITE,        struct SpritePrototype,       sprite_prototype) \
	X(REG_INTERACTABLE,  struct InteractablePrototype, interactable_prototype) \
	X(REG_ENTITY_PROTO,  struct EntityPrototype,       entity_prototype) \
	X(REG_ENTITY_INST,   struct EntityInstance,        entity_instance)

enum RegisterType{
	#define X(id, type, fncs) id,
	REGISTER_LIST
	#undef X
	REGISTER_COUNT
};

#define X(id, type, fncs) \
static inline type *grab_##id(struct RegisterManager *rman, int gindx){ \
	return (type *)e_grab_item(rman, id, gindx); \
} \
static inline type *load_##id(struct RegisterManager *rman, struct LoadData ld, int gindx){ \
	return (type *)e_load_item(rman, id, ld, gindx, sizeof(type)); \
} \
static inline void register_##id(struct RegisterManager *rman){ \
	e_load_register(rman, id, fncs()); \
}
REGISTER_LIST
#undef X

const char *regstr(enum RegisterType reg);
