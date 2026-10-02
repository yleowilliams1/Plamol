#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"
#include "entity.h"
#include "settings.h"

struct EntityManager *create_entity_manager(){
	struct EntityManager *e = XCALLOC(1, sizeof(struct EntityManager));

	return e;
}
void free_entity_manager(struct EntityManager *e){
	if(!e){LOG(IS_NULL, "Entity manager is NULL can't free");return;}
}

struct Object *create_object(){
	struct Object *obj = XCALLOC(1, sizeof(struct Object));

	return obj;
}
void free_object(struct Object *obj){
	if(!obj){LOG(IS_NULL, "Object is NULL can't free"); return;}
}

struct ItemProto *create_itemproto(){
	struct ItemProto *itm = XCALLOC(1, sizeof(struct ItemProto));
	return itm;
}
void free_itemproto(struct ItemProto *itm){
	if(!itm){LOG(IS_NULL, "ItemPrototype is NULL can't free");return;}
}
