#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"
#include "entity.h"
#include "inventory.h"
#define MAGIC "Y.M.C.A."
#define VERSION 1
bool write_object(struct SaveComponent *c, char *path){
	if(!c || !path){
		LOG(IS_NULL, "write_object called with NULL component or path");
		return false;
	}

	FILE *f = fopen(path, "wb");
	if(!f){
		LOG(NO_FILE, "write_object could not open %s", path);
		return false;
	}
	
	struct Header h = create_header(MAGIC, VERSION);
	if(fwrite(&h, sizeof(h), 1, f) != 1){fclose(f); return false;}
	if(!write_u32(f, c->uid)){fclose(f); return false;}
	if(!write_u32(f, c->flags)){fclose(f); return false;}
	if(!write_u32(f, (uint32_t)c->health_points)){fclose(f); return false;}
	if(!write_u32(f, (uint32_t)c->action_points)){fclose(f); return false;}
	if(!write_u32(f, (uint32_t)c->current_map)){fclose(f); return false;}
	if(!write_u32(f, (uint32_t)c->tx)){fclose(f); return false;}
	if(!write_u32(f, (uint32_t)c->ty)){fclose(f); return false;}
	if(!write_u8 (f, c->direction)){fclose(f); return false;}
	if(fwrite(&c->stats_save.acquired, sizeof(c->stats_save.acquired), 1, f) != 1){fclose(f); return false;}
	if(!write_u32(f, c->level)){fclose(f); return false;}
		
	if(c->inv.item_stack_count < 0 || (c->inv.item_stack_count > 0 && !c->inv.item_stack)){fclose(f); return false;}
	for(int i = 0; i < HOTBAR_SIZE; i++){
		if(!write_u32(f, (uint32_t)c->inv.hotbar_items[i].uid)){fclose(f); return false;}
		if(!write_u32(f, (uint32_t)c->inv.hotbar_items[i].count)){fclose(f); return false;}
	}
	if(!write_u16(f, (uint16_t)c->inv.item_stack_count)){fclose(f); return false;}
	for(int i = 0; i < c->inv.item_stack_count; i++){
		if(!write_u32(f, (uint32_t)c->inv.item_stack[i].uid)){fclose(f); return false;}
		if(!write_u32(f, (uint32_t)c->inv.item_stack[i].count)){fclose(f); return false;}
	}
	fclose(f);
	return true;	
}
