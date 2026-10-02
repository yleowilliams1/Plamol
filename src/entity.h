#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "util/util.h"

// Object hooks are a way for scripters to extend object functionality. These have default functionality which all objects have, but scripters can extend this. Theres no overrideing a object hooks defualt funciton 
#define OBJECT_HOOKS\
	X(SPAWN)\
	X(KILL)\
	X(MOVE)\
	X(INTERACT)\
	X(TICK)\
	X(LOAD)\
	X(DESTROY)\
	X(SEE)\
	X(OBSCURE)\
	X(COMBAT_START)\
	X(COMBAT_END)
// Concrete requests are a way for scripters to interact with object memory while keeping the engine authoriative. These are queued and then authorizated by the game engine. 
#define CONCRETE_REQUESTS\
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
	X(SET_VAR)\
	X(KILL_ARG)
// Object queries are ways for scripting to read object data. 
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
// Temporary recreation of Fallout 1s SPECIAL system before I do anything
#define OBJECT_STATS\
	X(STRENGTH)\
	X(PERCEPTION)\
	X(ENDURANCE)\
	X(CHARISMA)\
	X(INTELLIGENCE)\
	X(AGILITY)\
	X(LUCK)\
	X(AGE)\
	X(LEVEL)\
	X(CURRENT_EXP)\
	X(EXP_TO_NEXT_LEVEL)\
	X(CHAR_POINTS)\
	X(HIT_POINTS)\
	X(MAX_HITPOINTS)\
	X(ARMOR_CLASS)\
	X(ACTION_POINTS)\
	X(CARRY_WEIGHT)\
	X(MELEE_DAMAGE)\
	X(DAMAGE_RES)\
	X(POISON_RES)\
	X(RADIATION_RES)\
	X(SEQUENCE)\
	X(HEALING_RATE)\
	X(CRIT_CHANCE)\
	X(GUNS)\
	X(ENERGY_WEAPONS)\
	X(UNARMED)\
	X(MELEE_WEAPONS)\
	X(THROWING)\
	X(MEDICINE)\
	X(SNEAK)\
	X(THIEF)\
	X(SCIENCE)\
	X(REPAIR)\
	X(SPEECH)\
	X(BARTER)\
	X(GAMBLING)\
	X(OUTDOORSMAN)
#define FLAG_LIST\
	X(HOSTILE)\
	X(DEAD)\
	X(PASSTHROUGH)\
	X(LOOT)\
	X(IN_COMBAT)

enum ObjectDirection{
	#define X(name) DIR_##name,
	OBJECT_DIRECTION
	#undef X
};
enum ObjectStats{
	#define X(name) STAT_##name,
	OBJECT_STATS
	#undef X
	STAT_COUNT
};
enum ObjectFlags{
	#define X(name) FLG_##name,
	FLAG_LIST
	#undef X
};

// Proto are static contants in memory, actors store indirect index references to item props.

struct ItemProto{
	uint32_t uid;
	uint32_t name_sid;
	uint32_t descr_sid;
	int8_t stat_bonuses[STAT_COUNT];
};
struct ItemStack{
	uint32_t prop_uid;
	uint16_t count;
};
#define INVENTORY_SIZE 128
#define EQUIPMENT_SIZE 8

struct ObjectPrototype{
};
struct StaticComponent{
	uint32_t uid;
	uint32_t name_sid;
	uint32_t descr_sid;	
	int current_frame;
	float elapsed_time;
	
	uint32_t current_map;
	v2 tile_pos;	
	// Used for interaction	
	int rec_offset_x;
	int rec_offset_y;
	int rec_width;
	int rec_height;
};
struct ScriptableComponent{
	uint32_t stats[STAT_COUNT];
	int8_t stat_bonuses[STAT_COUNT];
	enum ObjectDirection dir;
	
	uint32_t object_flags;

	uint32_t hook_bitmask;

	struct ItemStack inv[INVENTORY_SIZE];
	struct ItemStack equ[EQUIPMENT_SIZE];
};
struct Object{
	struct StaticComponent root;
	struct ScriptableComponent sub;
};
struct EntityManager{
	/* We sacrificed the space of n*ptr for O(1) 
	 * complexity when we look for something. 
	 * Since we have our mutable and immutable
	 * gindexs; i.e just disk level data; we can
	 * load, unload, and get with just that index
	 * this way. And we keep unloaded data arr[gindex] = NULL*/
	
	// Indexed with uid
	struct Object **obj;
	
	// These are stored with UIDs in objects and are static. They can only be changed through the lua prototype file 
	struct ItemProto **item_proto;
	struct ObjectPrototype **obj_proto;
};

