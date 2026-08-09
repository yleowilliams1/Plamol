#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "t_gindex_tool.h"
#include "e_error_handler.h"
#include "i_inventory.h"
#include "m_map.h"
#include "p_entity.h"
#include "p_entity_instance.h"
#include "p_stats.h"
#include "p_instantiator.h"
#define E_POOL_SIZE 256

static struct EntityInstance einst[E_POOL_SIZE] = {0};

static bool find_einst_lindx(int *out);

struct EntityInstance *grab_entity(int GUID){
	for(int i = 0; i < E_POOL_SIZE; i++){
		if(einst[i].GUID == GUID){return &einst[i];}
	}
	
	ERR_LOG(ERR_NULL, "GUID %d is not loaded!", GUID);
	return NULL;
}

bool p_instantiate_entities(struct MapData m){
	// All entities get loaded
	// Don't forget to write the temp
	// save data
	int ent_cnt = m.meta[M_ENTITY_COUNT];	

	for(int i = 0; i < ent_cnt; i++){
		if(!m.entities[i].valid){continue;}
		int lindx;	
		bool found = find_einst_lindx(&lindx);	
		if(!found){return false;}
		
		struct EntityInstance *e = &einst[lindx];

		e->entity_gindx = m.entities[i].gindx;	
		e->GUID = m.entities[i].GUID;

		e->e = e_grab_entity(einst[lindx].entity_gindx, true);
		if(e->e.flags & ENT_INV){e->i = i_get_inv_proto(e->e.data[E_INV], true);}
		if(e->e.flags & ENT_STAT){e->s = s_grab_stats(e->e.data[E_STAT], true);}

		e->e.data[E_POSX] = m.entities[i].spawn_x;
		e->e.data[E_POSY] = m.entities[i].spawn_y;
		

		// Eventually you want to keep
		// a list of all loaded prototypes and then
		// bulk free after the loop but for now
		// this is fine.	
		e_free_entity(e->entity_gindx);
		i_free_inventory(e->e.data[E_INV]);
		t_free_stat(e->e.data[E_STAT]);
	}

	return true;
}
bool p_clear_entity_pool(){
	for(int i = 0; i < E_POOL_SIZE; i++){
		if(!einst[i].valid){continue;}
		einst[i] = (struct EntityInstance){0};	
	}
	return true;
}
static bool find_einst_lindx(int *out){
	for(int i = 0; i < E_POOL_SIZE; i++){
		if(einst[i].valid){continue;}
		einst[i].valid = true;
		*out = i;
		return true;
	}
	ERR_LOG(ERR_OUTOFBOUNDS, "Could not find free entity instance slot. Pool is FULL");
	return false;
}
