#include <stdio.h>
#include <stdlib.h>
#include "util/util.h"
#include "entity.h"
#include "map.h"
#include "settings.h"
#include "save.h"
static void free_item_immutable_at(struct ItemImmutable **it, int gindex);
static void free_entity_mutable_at(struct EntityMutable **mut, int gindex);

// I'm pretty sure these paths are NULL so this will fuck up so you should proably fix this

static void parse_entity_immutable(struct config_pack p, void *ptr){
	struct EntityImmutable *e = (struct EntityImmutable *)ptr;
	if(check(p.key, "name")){t_cpy(p.value, &e->name); return;}
	if(check(p.key, "description")){t_cpy(p.value, &e->description); return;}
	if(check(p.key, "dialogue_path")){t_cpy(p.value, &e->dialogue_path); return;}
	if(check(p.key, "script_path")){t_cpy(p.value, &e->script); return;}
	if(check(p.key, "sprite_gindex")){t_atoi(p.value, &e->sprite_gindex); return;}
	if(check(p.key, "starting_level")){t_atoi(p.value, &e->starting_level); return;}
	for(int i=0; i<BASE_STAT_COUNT; i++){
		if(check(p.key, (char *)base_stat_str(i))){t_atoi(p.value, &e->stats_array[i]); return;}
	}
}
static void parse_item_immutable(struct config_pack p, void *ptr){
	struct ItemImmutable *it = (struct ItemImmutable *)ptr;
	if(check(p.key, "name")){t_cpy(p.value, &it->name); return;}
	if(check(p.key, "description")){t_cpy(p.value, &it->description); return;}
	for(int i=0; i<BASE_STAT_COUNT; i++){
		if(check(p.key, (char *)base_stat_str(i))){t_atoi(p.value, &it->stats_array[i]); return;}
	}
}
struct EntityManager *create_entity_manager(){
	struct EntityManager *eman = XCALLOC(1, sizeof(struct EntityManager));
	// This will zero everything out. So nothing is loaded right now
	eman->immutable_entity = XCALLOC(INT(ENTITY_IMMUTABLE_COUNT), sizeof(struct EntityImmutable *));
	eman->immutable_item = XCALLOC(INT(ITEM_COUNT), sizeof(struct ItemImmutable *));
	eman->mutable_entity = XCALLOC(INT(ENTITY_INSTANCE_COUNT), sizeof(struct EntityMutable *));
	return eman;
}

void free_entity_manager(struct EntityManager **entity_manager){
	if (!entity_manager || !(*entity_manager)){return;}
	// This is what christ died for.
	for(int i=0; i<INT(ENTITY_IMMUTABLE_COUNT); i++){
		free_entity_immutable_at(&(*entity_manager)->immutable_entity[i], i);
	}
	for(int i=0; i<INT(ITEM_COUNT); i++){
		free_item_immutable_at(&(*entity_manager)->immutable_item[i], i);
	}
	for(int i=0; i<INT(ENTITY_INSTANCE_COUNT); i++){
		free_entity_mutable_at(&(*entity_manager)->mutable_entity[i], i);
	}
	free((*entity_manager)->immutable_entity);
	free((*entity_manager)->immutable_item);
	free((*entity_manager)->mutable_entity);

	(*entity_manager)->immutable_entity = NULL;
	(*entity_manager)->immutable_item = NULL;
	(*entity_manager)->mutable_entity = NULL;

	free(*entity_manager);
	*entity_manager = NULL;
}
void update_entity(struct EntityManager *entity_manager){
	if(!entity_manager){return;}
	// Update animation stuff here.
	// Frame 0 of animation 0 is the idle frame.
}
// Here is how loading happens. We take the map as an argument with it's list of
// mutable entities and load everything into memory. Then when the map is 
// unloaded we take its argument again, and we generate a list of the immutables 
// it used (entity protoype, itme prototype) and we remove those. And of course we
// remove the mutable entities as well. 
static struct ItemImmutable *load_item_immutable(int item_gindex){
	char *path = format_path(STR(ITEM_PATH), ".cfg", item_gindex);
	if(!path){
		LOG(IS_NULL, "Failed to build item path for gindex %d", item_gindex);
		return NULL;
	}
	struct ItemImmutable *it = XCALLOC(1, sizeof(struct ItemImmutable));
	if(!config(it, path, parse_item_immutable)){
		LOG(NO_FILE, "Failed to load item config at %s", path);
	}
	free(path);
	return it;
}
struct EntityImmutable *load_entity_immutable_from_disk(int prototype_gindex){
	char *path = format_path(STR(ENTITY_PATH), ".cfg", prototype_gindex);
	if(!path){
		LOG(IS_NULL, "Failed to build entity path for gindex %d", prototype_gindex);
		return NULL;
	}
	struct EntityImmutable *e = XCALLOC(1, sizeof(struct EntityImmutable));
	e->immutable_gindex = prototype_gindex;
	if(!config(e, path, parse_entity_immutable)){
		LOG(NO_FILE, "Failed to load entity config at %s", path);
	}
	free(path);
	return e;
}
static struct EntityMutable *load_entity_mutable_from_disk(int prototype_gindex, int instance_gindex, struct SaveManager *save, int save_file, struct EntityImmutable *immutable){
	struct EntityMutable *e = XCALLOC(1, sizeof(struct EntityMutable));
	if(get_mutable_from_save(save, STR(SAVE_PATH), save_file, instance_gindex, e)){return e;}
	// there was no save_file, so build a fresh mutable straight off the
	// prototype's immutable data. immutable is owned by the EntityManager
	// (see load_map_entities), we just read out of it here, never free it.
	e->mutable_gindex = instance_gindex;
	e->immutable_gindex = prototype_gindex;
	if(immutable){
		// Load default entity now
	}
	return e;
}
void update_entity_stats(struct EntityManager *eman, int entity_gindex){
	if(!eman){return;}
	eman->mutable_entity[entity_gindex];

}
void load_map_entities(struct EntityManager *eman, struct Map *map, struct SaveManager *save, int save_file){
	if(!eman || !map){return;}
	for(int i=0; i<map->entity_count; i++){
		struct Entity *e = &map->entity[i];
		if(!eman->immutable_entity[e->prototype_gindex]){
			eman->immutable_entity[e->prototype_gindex] = load_entity_immutable_from_disk(e->prototype_gindex);
		}
		if(!eman->mutable_entity[e->instance_gindex]){
			eman->mutable_entity[e->instance_gindex] = load_entity_mutable_from_disk(e->prototype_gindex, e->instance_gindex, save, save_file, eman->immutable_entity[e->prototype_gindex]);
			eman->mutable_entity[e->instance_gindex]->position.x = e->tx;
			eman->mutable_entity[e->instance_gindex]->position.y = e->ty;
		}
		// Loop through entity and load it's items if it's not already
		struct EntityMutable *mut = eman->mutable_entity[e->instance_gindex];
		if(mut){
			for(size_t s=0; s<INVENTORY_SIZE; s++){
				int ig = mut->inventory[s].item_gindex;
				if(mut->inventory[s].filled && !eman->immutable_item[ig]){
					eman->immutable_item[ig] = load_item_immutable(ig);
				}
			}
			for(size_t s=0; s<HOTBAR_SIZE; s++){
				int ig = mut->hotbar[s].item_gindex;
				if(mut->hotbar[s].filled && !eman->immutable_item[ig]){
					eman->immutable_item[ig] = load_item_immutable(ig);
				}
			}
		}
	}
}

void unload_map_entities(struct EntityManager *eman, struct Map *map){
	if(!eman || !map){return;}
	for(int i=0; i<map->entity_count; i++){
		struct Entity *e = &map->entity[i];

		struct EntityImmutable *imm = eman->immutable_entity[e->prototype_gindex];
		struct EntityMutable *mut = eman->mutable_entity[e->instance_gindex];

		// Free every item prototype this instance was carrying. We need
		// imm->inventory_size/hotbar_size to know how far to scan, so this
		// has to happen before imm itself gets freed below.
		if(mut && imm){
			for(size_t s=0; s< INVENTORY_SIZE; s++){
				if(mut->inventory[s].filled){
					free_item_immutable_at(&eman->immutable_item[mut->inventory[s].item_gindex], mut->inventory[s].item_gindex);
				}
			}
			for(size_t s=0; s< HOTBAR_SIZE; s++){
				if(mut->hotbar[s].filled){
					free_item_immutable_at(&eman->immutable_item[mut->hotbar[s].item_gindex], mut->hotbar[s].item_gindex);
				}
			}
		}

		free_entity_mutable_at(&eman->mutable_entity[e->instance_gindex], e->instance_gindex);
		free_entity_immutable_at(&eman->immutable_entity[e->prototype_gindex], e->prototype_gindex);
	}
}
void free_entity_immutable_at(struct EntityImmutable **e, int gindex){
	if(!*e){return;}
	if((*e)->name){free((*e)->name); (*e)->name = NULL;}
	if((*e)->description){free((*e)->description); (*e)->description = NULL;}
	if((*e)->dialogue_path){free((*e)->dialogue_path); (*e)->dialogue_path = NULL;}
	free(*e);
	*e = NULL;
}
static void free_item_immutable_at(struct ItemImmutable **it, int gindex){
	if(!*it){return;}
	if((*it)->name){free((*it)->name); (*it)->name = NULL;}
	if((*it)->description){free((*it)->description); (*it)->description = NULL;}
	free(*it);
	*it = NULL;
}
static void free_entity_mutable_at(struct EntityMutable **mut, int gindex){
	if(!*mut){return;}
	free(*mut);
	*mut = NULL;
}
