#pragma once
#include <stdint.h>
#include <stdbool.h>
#define HOOKS_LIST\
	X(ON_IDLE)\
	X(ON_MOVING)\
	X(ON_START_INTERACT)\
	X(ON_IN_INTERACT)\
	X(ON_END_INTERACT)\
	X(ON_DAMAGED)\
	X(ON_MISSED)\
	X(ON_DEATH)\
	X(ON_TURN_START)\
	X(ON_TURN_END)\
	X(ON_SHOOT_PISTOL)\
	X(ON_SHOOT_SMG)\
	X(ON_SHOOT_RIFLE)\
	X(ON_ATTACK_KNIFE)\
	X(ON_ATTACK_SLEDGEHAMMER)
enum HookType{
	#define X(name) name,
	HOOKS_LIST
	#undef X
	HOOK_COUNT
};
struct Hook{
	int animation;
	bool play_and_hold;
	uint32_t flag_add;
	uint32_t flag_remove;
};
struct Script;
struct Hook *resolve_hook(struct Script *script, enum HookType wanted);
