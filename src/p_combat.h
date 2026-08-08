#pragma once

struct CombatEntity{
	int entity_gindx;
};

struct CombatManager{
	struct CombatEntity *order;
	int current_order;
};
