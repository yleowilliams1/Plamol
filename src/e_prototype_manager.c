#include <stdio.h>
#include <stdbool.h>

#include "e_engine_settings.h"
#include "e_prototype_manager.h"

#include "t_log_handler.h"

struct PrototypeManager *e_create_pmanager(){
	struct PrototypeManager *pman = XCALLOC(1, sizeof(struct PrototypeManager));
	pman->reg= NULL;
	pman->reg = XCALLOC(1, sizeof(void **) * PROT_COUNT);
	
	// Allocate each types inner array	
	for(int i = 0; i < PROT_COUNT; i++){
		pman->reg[i] = XCALLOC(1, sizeof(void*) * e_grab_proto_itemcount(i));
	}
	
	return pman;
}
void e_free_pmanager(struct PrototypeManager *proto){
	if(!proto){LOG(LOG_NULL, "Can't free null prototype manager");return;}
	for(int i = 0; i < PROT_COUNT; i++){
		if(proto->reg[i] == NULL){continue;}
		e_free_register(proto, i);
	}

	free(proto);
	proto = NULL;
}
void e_load_register(struct PrototypeManager *proto, enum PrototypeFlag type, struct ItemFunctions fncs){
	if(!proto){LOG(LOG_NULL, "Can't load register %s with NULL prototype manager", protflag_str(type));return;}
	if(proto->reg){LOG(LOG_RELOAD, "Can't load %s register since it's not NULL", protflag_str(type)); return;}
	
	proto->reg[type] = XCALLOC(1, sizeof(void *) * e_grab_proto_itemcount(type));
	proto->fncs[type] = fncs;	
}
void e_free_register(struct PrototypeManager *proto, enum PrototypeFlag type){		
	if(!proto){LOG(LOG_NULL, "Can't free register %s with NULL prototype manager", protflag_str(type));return;}
	if(!proto->reg){LOG(LOG_RELOAD, "Can't free %s register since it's already NULL", protflag_str(type)); return;}
	
	for(int i = 0; i < e_grab_proto_itemcount(type); i++){
		if(!proto->reg[type][i]){continue;}
		e_free_item(proto, type, i);
	}	
	
	free(proto->reg[type]);
	proto->reg[type] = NULL;
	proto->fncs[type] = (struct ItemFunctions){0};	
}	
void *e_load_item(struct PrototypeManager *proto, enum PrototypeFlag type, int gindx, size_t size){
	if(!proto){LOG(LOG_NULL, "Can't load %d of %s since proto is null", gindx, protflag_str(type));return NULL;}	
	if(!proto->reg[type]){LOG(LOG_NULL, "Can't load item %d of %s since it's register is not loaded", gindx, protflag_str(type)); return NULL;}
	
	void *item = &proto->reg[type][gindx];
	struct ItemFunctions *fncs = &proto->fncs[type];

	if(item){LOG(LOG_RELOAD, "Tried to load item %d of %s but found it was not NULL. This is not a valid way of grabbing items from the register returning NULL", gindx, protflag_str(type)); return NULL;}
	
	item = XCALLOC(1, size);
	if(fncs->on_init){fncs->on_init(item);}	
	
	// Now just format the path
	char *full_path = t_format_path(e_grab_protopath(type), e_grab_protoformat(type), gindx);	
	if(!full_path){LOG(LOG_NULL, "Failed to format %s %d with path %s and format %s", protflag_str(type), gindx, e_grab_protopath(type), e_grab_protoformat(type)); /*Don't forget to free the item here*/return NULL;}
	
	// Now configure
	bool configured = t_config(item, full_path, fncs->on_load);
	if(!configured){
		LOG(LOG_NULL, "Failed to format %s %d with path %s and format %s", protflag_str(type), gindx, e_grab_protopath(type), e_grab_protoformat(type)); 
		/*Don't forget to free the item here*/
		if(full_path){free(full_path);}
		return NULL;
	}
	
	if(fncs->on_pload){fncs->on_pload(item);}

	free(full_path);
	return item;
}
void e_free_item(struct PrototypeManager *proto, enum PrototypeFlag type, int gindx){
	if(!proto){LOG(LOG_NULL, "Can't free %d of %s since proto is null", gindx, protflag_str(type));return ;}	
	if(!proto->reg[type]){LOG(LOG_NULL, "Can't free item %d of %s since it's register is not loaded", gindx, protflag_str(type)); return ;}

	void *item = &proto->reg[type][gindx];
	struct ItemFunctions *fncs = &proto->fncs[type];

	if(!item){LOG(LOG_RELOAD, "Tried to free item %d of %s but found it was NULL.", gindx, protflag_str(type)); return;}
	if(fncs->on_free){fncs->on_free(item);}
	free(item);
	item = NULL;
	return;
	
}
