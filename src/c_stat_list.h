#pragma once
#include <stdio.h>

#define BSTAT_LIST \
	X(STRENGTH) \
	X(DEXTERITY) \
	X(CONSITUTION) \
	X(SOCIAL) \
	X(INTELLIGENCE) \
	X(WISDOM)

#define DSTAT_LIST \
	X(PHYSICAL_COORDIANTION) \
	X(WORD) \
	X(PROBLEM_ANALYSIS) \
	X(SPATIAL) \
	X(MUSICAL) \
	X(NATURAL) \
	X(INTERPERSONAL) \
	X(INTRAPERSONAL) \
	X(INNOCENCE) \
	X(HEROISM) \
	X(LOVE) \
	X(AUTHORITARIAN) \
	X(ARMOR_CLASS) \
	X(MAX_HEALTH_POINTS) \
	X(MAX_ACIONT_POINTS) \
	X(INITATIVE_BONUS)

enum BaseStatEnum{
	#define X(name) name,
	BSTAT_LIST
	#undef X
	BASE_STAT_COUNT,
};
enum DerivedStatEnum{
	#define X(name) name,
	DSTAT_LIST
	#undef X
	DERIVED_STAT_COUNT,
};
enum StatType{
	BaseStatType,
	DerivedStatType,
	StatTypeCount,
};
const char *bsttr(enum BaseStatEnum bstat){
	switch(bstat){
		#define X(name) case name: return #name;
		BSTAT_LIST
		#undef X
		default: return NULL;
	}
}
const char *dsttr(enum DerivedStatEnum dstat){
	switch(dstat){
		#define X(name) case name: return #name;
		DSTAT_LIST
		#undef X
		default: return NULL;
	}
}
