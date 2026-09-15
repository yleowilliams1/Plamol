#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "escript.h"
#include "hook.h"
#include "util/util.h"
#define INVENTORY_SIZE 64
#define HOTBAR_SIZE 8
#define BASE_STAT_LIST\
	X(STRENGTH) \
	X(DEXTERITY) \
	X(SOCIAL) \
	X(INTELLIGENCE) \
	X(WILLPOWER) \
	X(PERCEPTION) \
	X(LOOKS)
enum BaseStat{
	#define X(name) name,
	BASE_STAT_LIST
	#undef X
	BASE_STAT_COUNT,
};
static inline const char *base_stat_str(enum BaseStat id){
	switch(id){
		#define X(name) case name: return #name;
		BASE_STAT_LIST
		#undef X
		default: return NULL;
	}
}
#define EXTRAS_LIST\
	X(HEALTH_PER_LEVEL) \
	X(AP_PER_LEVEL)\
	X(DAMAGE_BONUS) \
	X(ATTACK_BONUS) \
	X(GUNS)\
	X(UNARMED)\
	X(MELEE)\
	X(THROWING)\
	X(DOCTOR)\
	X(SNEAK)\
	X(STEAL)\
	X(SCIENCE)\
	X(REPAIR)\
	X(SPEECH)
enum ExtraInfo{
	#define X(name) name,
	EXTRAS_LIST
	#undef X
	EXTRAS_COUNT,
};
#define FLAG_LIST\
	X(HOSTILE)\
	X(DEAD)\
	X(PASSTHROUGH)\
	X(LOOT)
enum EntityFlags{
	#define X(name) name,
	FLAG_LIST
	#undef X
	ENTITY_FLAG_COUNT,
};
struct StorageItem{
	int item_gindex;	
	int count;
	bool filled;
};
// This is mostly safe to change but you do need to go to the save manager and add the new entries to the swap_entity_mutable. Keep pointers out of this so you can keep it a single fwrite
struct EntityMutable{
	uint32_t flags;
	
	int direction;
	int current_frame;
	float elapsed_time;
	
	int mutable_gindex;
	int immutable_gindex;

	int stats_modifier_array[BASE_STAT_COUNT];
	
	int current_health_points;
	int current_action_points;
	
	int level;

	struct StorageItem inventory[INVENTORY_SIZE];
	struct StorageItem hotbar[HOTBAR_SIZE];
	
	enum HookType active_hook;

	v2 position;
};
// This one gets loaded and never touched again
struct EntityImmutable{
	int stats_array[BASE_STAT_COUNT];
	int sprite_gindex;	
	int immutable_gindex;

	char *name;
	char *description;
	char *script_path;	
	struct Script *script;
	int starting_level;

	char *dialogue_path;
};
struct ItemImmutable{
	int stats_array[BASE_STAT_COUNT]; // flat stat bonus this item grants when equipped

	char *name;
	char *description;
};
struct EntityManager{
	/* We sacrificed the space of n*ptr for O(1) 
	 * complexity when we look for something. 
	 * Since we have our mutable and immutable
	 * gindexs; i.e just disk level data; we can
	 * load, unload, and get with just that index
	 * this way. And we keep unloaded data arr[gindex] = NULL*/
	struct EntityImmutable **immutable_entity;
	struct EntityMutable   **mutable_entity;
	struct ItemImmutable   **immutable_item;
};

// Live value of a base stat: prototype's base + this instance's modifiers +
// whatever's granted by items currently in its hotbar (treated as "equipped").
struct Map;
struct SaveManager;
struct SpriteManager;
struct EntityManager *create_entity_manager();
void update_entity(struct EntityManager *entity_manager, struct SpriteManager *sprite);
void free_entity_manager(struct EntityManager **entity_manager);
void load_map_entities(struct EntityManager *eman, struct Map *map, struct SaveManager *save, int save_file);
void unload_map_entities(struct EntityManager *eman, struct Map *map);
struct EntityImmutable *load_entity_immutable_from_disk(int prototype_gindex);
void free_entity_immutable_at(struct EntityImmutable **e, int gindex);
