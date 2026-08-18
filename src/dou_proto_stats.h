#pragma once
#include <stdbool.h>
#include "c_types.h"

struct BaseStats{
	int basestats[BSTAT_COUNT];
};

struct DouLoader;
struct DouLoader dou_stat();
