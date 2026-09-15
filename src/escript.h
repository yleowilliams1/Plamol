#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "hook.h"
struct Script{
	uint32_t hooks_bitmask;
	struct Hook *hooks[HOOK_COUNT];
};
struct EntityMutable;
struct EntityImmutable;
struct SpriteManager;
// Passed entity mutable and script into an update
struct Script *load_script_from_disk(char *path);
void free_script(struct Script **s);
void apply_script(struct EntityMutable *entity_mutable, struct EntityImmutable *entity_immutable, struct SpriteManager *sprites);
