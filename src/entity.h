#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

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
// Derived from BASE_STAT_LIST at load time (see compute_entity_bonuses in entity.c).
// Never read off disk directly - a prototype config file can only set the seven
// base stats above; these get (re)computed every time the immutable is loaded.
#define BONUS_LIST\
	X(INVESTIGATION_BONUS) \
	X(APPEAL_BONUS) \
	X(SPEECH_BONUS) \
	X(SMARTS_BONUS) \
	X(RANGED_BONUS) \
	X(ACTION_POINTS) \
	X(HEALTH_PER_LEVEL) \
	X(DAMAGE_BONUS) \
	X(ATTACK_BONUS)
enum Bonus{
	#define X(name) name,
	BONUS_LIST
	#undef X
	BONUS_COUNT,
};
#define FLAG_LIST\
	X(PLAYER)\
	X(HOSTILE)\
	X(DEAD)\
	X(PASSTHROUGH)\
	X(WORLD)\
	X(DOOR) 
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
struct EntityMutable{	
	float elapsed_time;
	int current_frame;
	int current_animation;
	int current_direction; // enum Dir - which direction-block of frames to draw
	
	int mutable_gindex;
	int immutable_gindex;

	int stats_modifier_array[BASE_STAT_COUNT];
	
	int current_health_points;
	int current_action_points;
	
	struct StorageItem inventory[INVENTORY_SIZE];
	struct StorageItem hotbar[HOTBAR_SIZE];
	
	uint32_t flags;
	uint32_t dialogue_flags;

	v2 position;
};
// This one gets loaded and never touched again
struct EntityImmutable{
	int stats_array[BASE_STAT_COUNT];
	int bonuses_array[BONUS_COUNT]; // derived - see compute_entity_bonuses(), never loaded from disk
	int sprite_gindex;	
	int immutable_gindex;

	char *name;
	char *description;

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
struct EntityManager *create_entity_manager();
void free_entity_manager(struct EntityManager **entity_manager);
void load_map_entities(struct EntityManager *eman, struct Map *map, struct SaveManager *save, int save_file);
void unload_map_entities(struct EntityManager *eman, struct Map *map);
int get_effective_stat(struct EntityManager *eman, struct EntityMutable *mut, enum BaseStat stat);
