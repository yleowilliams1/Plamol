#include <stdint.h>
#include <stdbool.h>
#include "util/util.h"
#include "stats.h"
#define MIN_STAT 1
#define MAX_STAT 20
#define MIN_AP 8
#define DEFAULT_STAT 5
#define MAX_SKILL_POINTS 5

static const int8_t SKILL_POINT_STAT_REQ[MAX_SKILL_POINTS] = {8, 12, 16, 18, 20};

static const enum StatsEnum SKILL_CORE_STATS[SK_SKILL_COUNT][2] = {
	[SK_GUNS]       = {ST_AGILITY,      ST_PERCEPTION},
	[SK_UNARMED]    = {ST_STRENGTH,     ST_AGILITY},
	[SK_DODGE]      = {ST_AGILITY,      ST_INTELLIGENCE},
	[SK_MELEE]      = {ST_STRENGTH,     ST_CONSTITUTION},
	[SK_INVESTIGATE]= {ST_PERCEPTION,   ST_WISDOM},
	[SK_DOCTOR]     = {ST_INTELLIGENCE, ST_WISDOM},
	[SK_SNEAK]      = {ST_WISDOM,       ST_PERCEPTION},
	[SK_THIEF]      = {ST_AGILITY,      ST_WISDOM},
	[SK_SPEECH]     = {ST_SOCIAL,       ST_BEAUTY},
	[SK_INTIMIDATE] = {ST_STRENGTH,     ST_CONSTITUTION},
	[SK_FLIRT]      = {ST_BEAUTY,       ST_SOCIAL},
	[SK_REPAIR]     = {ST_INTELLIGENCE, ST_WISDOM},
	[SK_SCIENCE]    = {ST_INTELLIGENCE, ST_PERCEPTION},
	[SK_BULWARK]    = {ST_STRENGTH,     ST_CONSTITUTION},
	[SK_ATTRACTION] = {ST_CONSTITUTION, ST_BEAUTY},
	[SK_CONVIVIAL]  = {ST_CONSTITUTION, ST_SOCIAL},
};

static int max_i(int a, int b){return a > b ? a : b;}
static int floor_half(int v){return v >= 0 ? v / 2 : -((-v + 1) / 2);}

int8_t get_effective_stat(enum StatsEnum type, struct StatsProto *p, struct StatsSave *s){
	if(!p){LOG(IS_NULL, "Stats prototype is NULL");return DEFAULT_STAT;}
	if(!s){LOG(IS_NULL, "Stats save is NULL");return DEFAULT_STAT;}
	if(type < 0 || type >= ST_STAT_COUNT){LOG(IS_NULL, "%d Is not a valid enum type", type);return DEFAULT_STAT;}

	int8_t innate = p->innate.stats[type];
	int8_t acquired = s->acquired.stats[type];
	if(innate > MAX_STAT || innate < 0){LOG(IS_NULL, "Innate (prototype) value invalid for %d: %d", type, innate); return DEFAULT_STAT;}
	if(acquired > MAX_STAT || acquired < 0){LOG(IS_NULL, "Acquired (save) value invalid for %d: %d", type, acquired); return DEFAULT_STAT;}
	if((innate + acquired) > MAX_STAT){LOG(IS_NULL, "Innate + acquired exceeds max stat %d: %d", MAX_STAT, innate + acquired);if(innate > acquired){return innate;}else{return acquired;}}
	if((innate + acquired) < MIN_STAT){LOG(IS_NULL, "Innate + acquired is below min stat %d: %d", MIN_STAT, innate + acquired); return DEFAULT_STAT;}
	return innate + acquired;
}
int8_t get_skill_points(enum SkillsEnum type, struct StatsProto *p, struct StatsSave *s){
	if(!p){LOG(IS_NULL, "Stats prototype is NULL");return DEFAULT_STAT;}
	if(!s){LOG(IS_NULL, "Stats save is NULL");return DEFAULT_STAT;}
	if(type < 0 || type >= SK_SKILL_COUNT){LOG(IS_NULL, "%d Is not a valid enum type", type);return DEFAULT_STAT;}

	int8_t innate = p->innate.skills[type];
	int8_t acquired = s->acquired.skills[type];
	if(innate > MAX_SKILL_POINTS || innate < 0){LOG(IS_NULL, "Innate (prototype) value invalid for %d: %d", type, innate); return DEFAULT_STAT;}
	if(acquired > MAX_SKILL_POINTS || acquired < 0){LOG(IS_NULL, "Acquired (save) value invalid for %d: %d", type, acquired); return DEFAULT_STAT;}
	if((innate + acquired) > MAX_STAT){LOG(IS_NULL, "Innate + acquired exceeds max stat %d: %d", MAX_STAT, innate + acquired);if(innate > acquired){return innate;}else{return acquired;}}
	return innate + acquired;
}
struct StatBlock calc_skill_bases(struct StatsProto *p, struct StatsSave *s){
	if(!p){LOG(IS_NULL, "Stats prototype is NULL");return (struct StatBlock){0};}
	if(!s){LOG(IS_NULL, "Stats save is NULL");return (struct StatBlock){0};}
	struct StatBlock out = {0};
	for(int i = 0; i < ST_STAT_COUNT; i++){
		out.stats[i] = get_effective_stat((enum StatsEnum)i, p, s);
	}
	for(int i = 0; i < SK_SKILL_COUNT; i++){
		int a = out.stats[SKILL_CORE_STATS[i][0]];
		int c = out.stats[SKILL_CORE_STATS[i][1]];
		out.skills[i] = (int8_t)((a + c + 1) / 2); // ceil((a+c)/2)
	}
	return out;
}
int32_t get_derived_stat(enum DerivedStatsEnum type, struct StatsProto *p, struct StatsSave *s, int level){
	if(!p){LOG(IS_NULL, "Stats prototype is NULL");return 0;}
	if(!s){LOG(IS_NULL, "Stats save is NULL");return 0;}
	if(type < 0 || type >= DE_DERIVED_COUNT){LOG(IS_NULL, "%d Is not a valid enum type", type);return 0;}
	if(level < 1){LOG(IS_NULL, "Invalid level %d", level);return 0;}

	int str = get_effective_stat(ST_STRENGTH, p, s);
	int agi = get_effective_stat(ST_AGILITY, p, s);
	int con = get_effective_stat(ST_CONSTITUTION, p, s);
	int per = get_effective_stat(ST_PERCEPTION, p, s);
	int wis = get_effective_stat(ST_WISDOM, p, s);

	switch(type){
		case DE_MAX_HEALTH:                return (max_i(str, con) + max_i(agi, wis)) * level;
		case DE_MAX_AP:                    return max_i(max_i(agi, per) - 8, MIN_AP);
		case DE_INITATIVE_BONUS:           return agi - 8;
		case DE_MAXIMUM_WEIGHT:            return str * 25 + 75;
		case DE_NATURAL_MELEE_DAMAGE:      return floor_half(str - 8);
		case DE_NATURAL_ARMOR_CLASS:       return floor_half(agi - 8);
		case DE_NATURAL_DAMAGE_RESISTANCE: return floor_half(con - 8);
		case DE_NATURAL_POISON_RESISTANCE: return floor_half(wis - 8);
		default: break;
	}
	return 0;
}

bool add_stat_point(enum StatsEnum type, struct StatsProto *p, struct StatsSave *s){
	if(!p){LOG(IS_NULL, "Stats prototype is NULL");return false;}
	if(!s){LOG(IS_NULL, "Stats save is NULL");return false;}
	if(type < 0 || type >= ST_STAT_COUNT){LOG(IS_NULL, "%d Is not a valid enum type", type);return false;}

	int innate = p->innate.stats[type];
	int acquired = s->acquired.stats[type];
	if(innate < 0 || innate > MAX_STAT || acquired < 0 || acquired > MAX_STAT){LOG(IS_NULL, "Invalid stat values for %d: %d/%d", type, innate, acquired);return false;}
	if(innate + acquired >= MAX_STAT){LOG(IS_NULL, "Stat %d already at max %d", type, MAX_STAT);return false;}
	s->acquired.stats[type]++;
	return true;
}

bool add_skill_point(enum SkillsEnum type, struct StatsProto *p, struct StatsSave *s){
	if(!p){LOG(IS_NULL, "Stats prototype is NULL");return false;}
	if(!s){LOG(IS_NULL, "Stats save is NULL");return false;}
	if(type < 0 || type >= SK_SKILL_COUNT){LOG(IS_NULL, "%d Is not a valid enum type", type);return false;}

	int acquired = s->acquired.skills[type];
	if(acquired < 0 || acquired > MAX_SKILL_POINTS){LOG(IS_NULL, "Acquired (save) value invalid for %d: %d", type, acquired);return false;}
	if(acquired >= MAX_SKILL_POINTS){LOG(IS_NULL, "Skill %d already at max bonus %d", type, MAX_SKILL_POINTS);return false;}

	int need = SKILL_POINT_STAT_REQ[acquired];
	int a = get_effective_stat(SKILL_CORE_STATS[type][0], p, s);
	int c = get_effective_stat(SKILL_CORE_STATS[type][1], p, s);
	if(a < need && c < need){
		LOG(IS_NULL, "Skill %d needs a core stat of %d (have %d and %d)", type, need, a, c);
		return false;
	}
	s->acquired.skills[type]++;
	return true;
}
