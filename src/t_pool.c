#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "c_magic_number.h"
#include "t_log_handler.h"
#include "t_pool.h"


static struct PoolHeader *slot_at(struct Pool *p, int i);

void t_pool_init(struct Pool *p,int cap, size_t element_size){
	p->items = XCALLOC(1, element_size * cap);
	p->element_size = element_size;
	p->cap = cap;
	p->count = 0;

	for(int i = 0; i < cap; i++){
		struct PoolHeader *h = slot_at(p, i);
		h->slot = (uint32_t)i;
		h->gen = VALID_GEN_ID;
		h->active = false;
	}
	
	LOG(LOG_LOAD, "Initialized pool %p", p);
}
void  t_pool_free_all(struct Pool *p){
	if(!p){LOG(LOG_FREE, "Can't free NULL pool"); return;}	
	if(p->items){free(p->items); p->items = NULL;}
	memset(p, 0, sizeof(struct Pool));
}
void *t_pool_alloc(struct Pool *p, struct InRef *out){
	for(int i = 0; i < p->cap; i++){
		struct PoolHeader *h = slot_at(p, i);
		if(h->active){continue;}
		
		uint32_t slot = h->slot;
		uint32_t gen = h->gen;	
		memset(h, 0, p->element_size);
		h->slot = slot;
		h->gen = gen;
		h->active = true;

		p->count++;
		if(out){ *out = (struct InRef){.slot = slot, .gen = gen};}
		return h;	
	}
	LOG(LOG_ALLOC, "Cann't allocate, Pool is full (cap %d)", p->cap);
	if(out){*out = (struct InRef){0};}
	return NULL;
}
void *t_pool_get(struct Pool *p, struct InRef r){
    	if(r.slot >= (uint32_t)p->cap){ LOG(LOG_OUTOFBOUNDS, "Tried to get pool item of slot %d while cap is %d", r.slot, p->cap);return NULL; }
    	struct PoolHeader *h = slot_at(p, (int)r.slot);
    	if(!h->active || h->gen != r.gen){ return NULL; }
    	return h;
}

bool t_pool_release(struct Pool *p, struct InRef r){
    	struct PoolHeader *h = t_pool_get(p, r);
    	if(!h){ return false; }
    	h->active = false;
    	h->gen++;// every existing ref to this slot is now stale
    	p->count--;
   	return true;
}
void *t_pool_next(struct Pool *p, int *cursor, struct InRef *out){
    	while(*cursor < p->cap){
        	struct PoolHeader *h = slot_at(p, *cursor);
        	(*cursor)++;
        	if(!h->active){ continue; }
        	if(out){ *out = (struct InRef){ .slot = h->slot, .gen = h->gen }; }
        	return h;
    	}
   	return NULL;
}





static struct PoolHeader *slot_at(struct Pool *p, int i){
	return(struct PoolHeader *)((char *)p->items + (size_t)i * p->element_size);
}
