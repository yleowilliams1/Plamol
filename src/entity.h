#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "util/util.h"
#include "string_table.h"
// hooks are a way for scripters to extend object functionality. These have default functionality , but scripters can extend this. Theres no overrideing a hooks defualt funciton 
// requests are a way for scripters to interact with object memory while keeping the engine authoriative. These are queued and then authorizated by the game engine. 

#define ITEM_HOOK\
	X(ON_USE)\
	X(ON_INSPECT)\
	X(ON_MOVE)\
	X(ON_ATTACK)\
	X(ON_DROP)\
	X(ON_CONSUME)\
	X(ON_LOAD)
#define ITEM_REQUEST\
	X(TRY_ATTACK)\
	X(TRY_HEAL)\
	X(TRY_EFFECT)
#define OBJECT_HOOKS\
	X(ON_SPAWN)\
	X(ON_KILL)\
	X(ON_MOVE)\
	X(ON_INTERACT)\
	X(ON_TICK)\
	X(ON_LOAD)\
	X(ON_DESTROY)\
	X(ON_SEE)\
	X(ON_OBSCURE)\
	X(ON_COMBAT_START)\
	X(ON_COMBAT_END)
#define OBJECT_REQUESTS\
	X(TRY_SPAWN)\
	X(TRY_KILL)\
	X(TRY_DESPAWN)\
	X(TRY_FLOATER)\
	X(TRY_DIALOGUE)\
	X(TRY_ANIMATE)\
	X(TRY_HIDE)\
	X(TRY_COLLIDE)\
	X(TRY_PASSTHROUGH)\
	X(TRY_ATTACK)\
	X(TRY_USE_ABILITY)\
	X(TRY_DAMAGE)\
	X(TRY_HEAL)\
	X(TRY_EFFECT)\
	X(TRY_REMOVE_EFFECT)\
	X(TRY_FACE)\
	X(TRY_CLEAR_QUEUE)\
	X(TRY_QUEUE_MOVE)\
	X(TRY_GET_ITEM)\
	X(TRY_DROP_ITEM)\
	X(TRY_EQUIP)\
	X(TRY_UNEQUIP)\
	X(TRY_GIVE_XP)\
	X(TRY_INTERACT)\
	X(TRY_SET_VAR)
#define OBJECT_QUERIES\
	X(GET_IN_COMBAT)\
	X(GET_POSITION)\
	X(GET_UID)\
	X(GET_NAME)\
	X(GET_CONCRETE_CLASS)\
	X(GET_HP)\
	X(GET_MAX_HP)\
	X(GET_IS_DEAD)\
	X(GET_LEVEL)\
	X(GET_STAT)\
	X(GET_TARGET)\
	X(GET_EFFECT)\
	X(GET_HOSTILE)\
	X(GET_FACING)\
	X(GET_AREA)\
	X(GET_DISTANCE)\
	X(GET_CAN_SEE)\
	X(GET_NEAREST)\
	X(GET_HAS_ITEM)\
	X(GET_EQUIPPED)\
	X(GET_VAR)\
	X(GET_IS_BUSY)	
#define OBJECT_DIRECTION\
	X(N)\
	X(S)\
	X(E)\
	X(W)\
	X(NW)\
	X(NE)\
	X(SW)\
	X(SE)
#define OBJECT_STATS\
	X(STRENGTH)\
	X(AGILITY)\
	X(SOCIAL)\
	X(CONSTITUTION)\
	X(PERCEPTION)\
	X(INTELLIGENCE)\
	X(WISDOM)\
	X(BEAUTY)
#define OBJECT_SKILLS\
	X(GUNS)\
	X(UNARMED)\
	X(DODGE)\
	X(MELEE_WEAPON)\
	X(INVESTIGATE)\
	X(DOCTOR)\
	X(SNEAK)\
	X(THIEF)\
	X(SPEECH)\
	X(INTIMIDATE)\
	X(FLIRT)\
	X(REPAIR)\
	X(SCIENCE)
#define OBJECT_RUNTIME\
	X(AGE)\
	X(LEVEL)\
	X(CURRENT_XP)\
	X(CHAR_POINTS)\
	X(HIT_POINTS)\
	X(ARMOR_CLASS)
#define OBJECT_DERIVED\
	X(MAX_HEALTH)\
	X(MAX_AP)\
	X(CARRY_WEIGHT)\
	X(NATURAL_AC)\
	X(NATURAL_MELEE_DAMAGE)\
	X(NATURAL_DAMAGE_RES)\
	X(NATURAL_POISON_RES)\
	X(NATURAL_RADIATION_RES)\
	X(NATURAL_HEALING_RATE)
#define OBJECT_FLAGS\
	X(HOSTILE)\
	X(DEAD)\
	X(PASSTHROUGH)\
	X(LOOT)\
	X(IN_COMBAT)
enum ItemHooks{
	#define X(name) ITEM_##name,
	ITEM_HOOKS
	#undef X
};
enum ItemRequest{
	#define X(name) ITEM_##name,
	ITEM_REQUESTS
	#undef X
};
enum ObjectHooks{
	#define X(name) OBJ_##name,
	OBJECT_HOOK
	#undef X
};
enum ObjectRequest{
	#define X(name) OBJ_##name,
	OBJECT_REQUEST
	#undef X
};
enum ObjectQueries{
	#define X(name) OBJ_##name,
	OBJECT_QUERIES
	#undef X
};
enum ObjectDirection{
	#define X(name) OBJ_##name,
	OBJECT_DIRECTION
	#undef X
};
enum ObjectStats{
	#define X(name) OBJ_##name,
	OBJECT_STATS
	#undef X
	OBJ_STAT_COUNT
};
enum ObjectSkills{
	#define X(name) OBJ_##name,
	OBJECT_SKILLS
	#undef X
	OBJ_SKILL_COUNT
};
enum ObjectRuntime{
	#define X(name) OBJ_##name,
	OBJECT_RUNTIME
	#undef X
	OBJ_RUNTIME_COUNT
};
enum ObjectDerived{
	#define X(name) OBJ_##name,
	OBJECT_DERIVED
	#undef X
	OBJ_DERIVED_COUNT
};
enum ObjectFlags{
	#define X(name) OBJ_##name,
	OBJECT_FLAG
	#undef X
};
struct ItemProto{
	uint32_t uid;
	uint32_t name_str;
	uint32_t descr_str;
	uint32_t weight;
	uint8_t attack_damage;
	uint8_t skill_to_use;
	uint16_t value;	
	int8_t stat_bonuses[OBJ_STAT_COUNT];
	uint32_t available_hooks;
};
struct ItemStack{
	uint32_t proto_uid;
	uint16_t count;
};
#define EQUIPMENT_SIZE 8

// Seperate into runtime, saved, and proto and then build a inref for the entity.

struct Effect{
	uint8_t effect_type;
	float elapsed_time;
	bool active;
};	
#define NUMBER_OF_EFFECTS 16
struct SavedComponent{
	// We don't store derived stats, instead we load them live so we don't have to worry about the data gfoing out of sync.
	uint32_t flags;

	uint32_t health_points;
	uint32_t action_points;
	
	uint32_t current_map;

	uint32_t tx;
	uint32_t ty;

	uint8_t direction;

	// These mutable bonuses are the exception. For items and effects you have procedural control over what gets removed when and how, where as for mutable bonuses you don't have that. This is used for things like leveling up, the player character uses this on character creation to modify the default player prototype. Generally, you want to stay away from this if you can for thigns that aren't leveling.  

	int8_t stat_mutable_bonus[OBJ_STAT_COUNT];	
	int8_t skill_mutable_bonus[OBJ_SKILL_COUNT];
	int8_t derived_mutable_bonus[OBJ_DERIVED_COUNT];

	uint32_t runtime_data[OBJ_RUNTIME_COUNT];
	
	struct Effect effects[NUMBER_OF_EFFECTS];
	
	uint32_t inventory_size;
	uint32_t equipment_size;
	struct ItemStack *inv;
	struct ItemStack *equ;
	
};
struct RuntimeComponent{
	vf2 tile_offset;
	int current_frame;
	float elapsed_time;

};
struct ProtoComponent{
	uint32_t uid;
	uint32_t name_string;
	uint32_t descr_string;	
	
	uint32_t sprite_id;
	char *dialogue_name;
	char *script_name;

	int8_t stats[OBJ_STAT_COUNT];	
	int8_t skill[OBJ_SKILL_COUNT];

	// Used for interaction	
	int rec_offset_x;
	int rec_offset_y;
	int rec_width;
	int rec_height;
	
	// What effects can be applied
	uint32_t effect_bitmask;
	uint32_t available_hooks;
};

struct EntityManager{
	/* We sacrificed the space of n*ptr for O(1) 
	 * complexity when we look for something. 
	 * Since we have our mutable and immutable
	 * gindexs; i.e just disk level data; we can
	 * load, unload, and get with just that index
	 * this way. And we keep unloaded data arr[gindex] = NULL*/
	
	// Loaded with UI	
	struct ScriptableComponent **object_scriptables;
	struct ObjectProto **object_prototypes;
	
	// These are stored with UIDs in objects and are static. They can only be changed through the lua prototype file 
	struct ItemProto **item_proto;
};

