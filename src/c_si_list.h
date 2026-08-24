#pragma once

#define SI_LIST \
	X(SI_FLAGS) \
	X(ESI_INPUT) \
	X(ESI_MAP) \
	X(ESI_WORLD)

enum SiEnum{
	#define X(name) name,
	SI_LIST
	#undef X
	SI_COUNT,
};
const char *sistr(enum SiEnum si);
