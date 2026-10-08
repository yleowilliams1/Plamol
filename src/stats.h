#pragma once
#include <stdint.h>
#include <stdbool.h>

#define STATS_LIST\
	X(STRENGTH)\
	X(AGILITY)\
	X(SOCIAL)\
	X(CONSTITUTION)\
	X(PERCEPTION)\
	X(INTELLIGENCE)\
	X(WISDOM)\
	X(BEAUTY)
#define SKILLS_LIST\
	X(GUNS)\
	X(UNARMED)\
	X(DODGE)\
	X(MELEE)\
	X(INVESTIGATE)\
	X(DOCTOR)\
	X(SNEAK)\
	X(THIEF)\
	X(SPEECH)\
	X(INTIMIDATE)\
	X(FLIRT)\
	X(REPAIR)\
	X(SCIENCE)\
	X(BULWARK)\
	X(ATTRACTION)\
	X(CONVIVIAL)
#define DERIVED_LIST\
	X(MAX_HEALTH)\
	X(MAX_AP)\
	X(INITATIVE_BONUS)\
	X(MAXIMUM_WEIGHT)\
	X(NATURAL_MELEE_DAMAGE)\
	X(NATURAL_ARMOR_CLASS)\
	X(NATURAL_DAMAGE_RESISTANCE)\
	X(NATURAL_POISON_RESISTANCE)
enum StatsEnum{
	#define X(name) ST_##name,
	STATS_LIST
	#undef X
	ST_STAT_COUNT
};
enum SkillsEnum{
	#define X(name) SK_##name,
	SKILLS_LIST
	#undef X
	SK_SKILL_COUNT
};
enum DerivedStatsEnum{
	#define X(name) DE_##name,
	DERIVED_LIST
	#undef X
	DE_DERIVED_COUNT
};
struct StatBlock{
	int8_t stats[ST_STAT_COUNT];
	int8_t skills[SK_SKILL_COUNT];
};
struct StatsProto{
	// What the prototype grants
	struct StatBlock innate;
};
struct StatsSave{
	// What was gained after the prototype: effects, leveling, character creation
	struct StatBlock acquired;	
};

int8_t get_effective_stat(enum StatsEnum type, struct StatsProto *p, struct StatsSave *s);
int8_t get_skill_points(enum SkillsEnum type, struct StatsProto *p, struct StatsSave *s);
struct StatBlock calc_skill_bases(struct StatsProto *p, struct StatsSave *s);
int32_t get_derived_stat(enum DerivedStatsEnum type, struct StatsProto *p, struct StatsSave *s, int level);
bool add_stat_point(enum StatsEnum type, struct StatsProto *p, struct StatsSave *s);
bool add_skill_point(enum SkillsEnum type, struct StatsProto *p, struct StatsSave *s);
