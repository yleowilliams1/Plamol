#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include "settings.h"
#include "util/util.h"
#include "entity.h"
#include "inventory.h"
#define PACKET_PATH_MAX 512
#define MAGIC "BabyBlue"
#define VERSION 1

_Static_assert(ST_STAT_COUNT < 256 && SK_SKILL_COUNT < 256, "stat/skill counts are saved as u8");

static bool write_stat_block(FILE *f, const struct StatBlock *b){
	if(!write_u8(f, (uint8_t)ST_STAT_COUNT)){return false;}
	if(!write_u8(f, (uint8_t)SK_SKILL_COUNT)){return false;}
	for(int i = 0; i < ST_STAT_COUNT; i++){
		if(!write_u8(f, (uint8_t)b->stats[i])){return false;}
	}
	for(int i = 0; i < SK_SKILL_COUNT; i++){
		if(!write_u8(f, (uint8_t)b->skills[i])){return false;}
	}
	return true;
}

bool write_object(struct SaveComponent *c, uint32_t slot){
	if(!c){LOG(IS_NULL, "write_object called with NULL component"); return false;}

	const char *path = STR(SAVE_PATH);
	if(!path){LOG(IS_NULL, "SAVE_PATH not set"); return false;}

	// Validate everything before touching the disk, so a bad object can't
	// leave a half-written file behind.
	if(c->inv.item_stack_count < 0 || (c->inv.item_stack_count > 0 && !c->inv.item_stack)){
		LOG(WRITE, "Object %" PRIu32 " has an invalid inventory (stack count %d)", c->uid, (int)c->inv.item_stack_count);
		return false;
	}

	size_t len = strlen(path);
	const char *sep = (len > 0 && path[len - 1] == '/') ? "" : "/";

	char full_path[PACKET_PATH_MAX];
	if(!t_snprintf(full_path, sizeof(full_path), NULL, "%s%spackets/%" PRIu32 "-%" PRIu32 ".packet", path, sep, c->uid, slot)){
		LOG(WRITE, "Packet path too long for object %" PRIu32, c->uid);
		return false;
	}

	// "x" = exclusive create: fails if the file already exists, so a packet
	// that another save references can never be overwritten.
	FILE *f = fopen(full_path, "wbx");
	if(!f){
		int err = errno;
		if(err == EEXIST){
			LOG(WRITE, "Refusing to overwrite existing packet %s", full_path);
		}else{
			LOG(NO_FILE, "Could not create packet %s: %s", full_path, strerror(err));
		}
		// Return here, never go to fail: the file in this case isn't ours
		// and the cleanup below would delete somebody else's packet.
		return false;
	}

	struct Header h = create_header(MAGIC, VERSION);
	if(!write_header(f, &h)){goto fail;}
	if(!write_u32(f, c->uid)){goto fail;}
	if(!write_u32(f, c->flags)){goto fail;}
	if(!write_u32(f, (uint32_t)c->health_points)){goto fail;}
	if(!write_u32(f, (uint32_t)c->action_points)){goto fail;}
	if(!write_u32(f, (uint32_t)c->current_map)){goto fail;}
	if(!write_u32(f, (uint32_t)c->tx)){goto fail;}
	if(!write_u32(f, (uint32_t)c->ty)){goto fail;}
	if(!write_u8 (f, c->direction)){goto fail;}
	if(!write_stat_block(f, &c->stats_save.acquired)){goto fail;}
	if(!write_u32(f, c->level)){goto fail;}

	for(int i = 0; i < HOTBAR_SIZE; i++){
		if(!write_u32(f, (uint32_t)c->inv.hotbar_items[i].uid)){goto fail;}
		if(!write_u32(f, (uint32_t)c->inv.hotbar_items[i].count)){goto fail;}
	}
	if(!write_u16(f, (uint16_t)c->inv.item_stack_count)){goto fail;}
	for(int i = 0; i < c->inv.item_stack_count; i++){
		if(!write_u32(f, (uint32_t)c->inv.item_stack[i].uid)){goto fail;}
		if(!write_u32(f, (uint32_t)c->inv.item_stack[i].count)){goto fail;}
	}

	// Buffered writes can fail at flush/close time (disk full), so these
	// count as part of the write, not cleanup.
	if(fflush(f) != 0){goto fail;}
	if(fclose(f) != 0){f = NULL; goto fail;}
	return true;

fail:
	LOG(WRITE, "Failed writing packet %s", full_path);
	if(f){fclose(f);}
	// Safe to delete: "wbx" guarantees this call created the file.
	remove(full_path);
	return false;
}

void free_save_component(struct SaveComponent *c){
	if(!c){return;}
	free(c->inv.item_stack);
	c->inv.item_stack = NULL;
	c->inv.item_stack_count = 0;
}

// Counterpart of write_stat_block. The stored counts must match this build's
// stat/skill lists exactly: the lists are X-macros, so adding or reordering an
// entry would silently shift every value after it. Bump VERSION when that happens.
static bool read_stat_block(FILE *f, struct StatBlock *b){
	uint8_t stat_count, skill_count;
	if(!read_u8(f, &stat_count)){return false;}
	if(!read_u8(f, &skill_count)){return false;}
	if(stat_count != ST_STAT_COUNT || skill_count != SK_SKILL_COUNT){
		LOG(PARSE, "Stat block has %u stats / %u skills, this build expects %d / %d",
			(unsigned)stat_count, (unsigned)skill_count, (int)ST_STAT_COUNT, (int)SK_SKILL_COUNT);
		return false;
	}
	struct StatBlock out = {0};
	for(int i = 0; i < ST_STAT_COUNT; i++){
		if(!read_i8(f, &out.stats[i])){return false;}
	}
	for(int i = 0; i < SK_SKILL_COUNT; i++){
		if(!read_i8(f, &out.skills[i])){return false;}
	}
	*b = out;
	return true;
}

// Mirrors write_object, field for field.
bool read_object(struct SaveComponent *c, uint32_t uid, uint32_t slot){
	if(!c){LOG(IS_NULL, "read_object called with NULL component"); return false;}

	const char *path = STR(SAVE_PATH);
	if(!path){LOG(IS_NULL, "SAVE_PATH not set"); return false;}
	size_t len = strlen(path);
	const char *sep = (len > 0 && path[len - 1] == '/') ? "" : "/";

	char full_path[PACKET_PATH_MAX];
	if(!t_snprintf(full_path, sizeof(full_path), NULL, "%s%spackets/%" PRIu32 "-%" PRIu32 ".packet", path, sep, uid, slot)){
		LOG(READ, "Packet path too long for object %" PRIu32, uid);
		return false;
	}

	FILE *f = fopen(full_path, "rb");
	if(!f){
		int err = errno;
		LOG(NO_FILE, "Could not open packet %s: %s", full_path, strerror(err));
		return false;
	}

	// Built up in a local so a half-read object never reaches the caller.
	struct SaveComponent o = {0};
	struct Header h;
	uint32_t file_uid = 0;
	uint16_t stack_count = 0;
	size_t remaining = 0;

	if(!read_header(f, MAGIC, VERSION, &h)){goto fail;}
	if(!read_u32(f, &file_uid)){goto fail;}
	if(file_uid != uid){
		LOG(PARSE, "Packet %s holds object %" PRIu32 ", expected %" PRIu32, full_path, file_uid, uid);
		goto fail;
	}
	o.uid = file_uid;
	if(!read_u32(f, &o.flags)){goto fail;}
	if(!read_i32(f, &o.health_points)){goto fail;}
	if(!read_i32(f, &o.action_points)){goto fail;}
	if(!read_i32(f, &o.current_map)){goto fail;}
	if(!read_i32(f, &o.tx)){goto fail;}
	if(!read_i32(f, &o.ty)){goto fail;}
	if(!read_u8 (f, &o.direction)){goto fail;}
	if(!read_stat_block(f, &o.stats_save.acquired)){goto fail;}
	if(!read_u32(f, &o.level)){goto fail;}

	for(int i = 0; i < HOTBAR_SIZE; i++){
		if(!read_i32(f, &o.inv.hotbar_items[i].uid)){goto fail;}
		if(!read_i32(f, &o.inv.hotbar_items[i].count)){goto fail;}
	}
	if(!read_u16(f, &stack_count)){goto fail;}
	if(stack_count > INT16_MAX){
		LOG(PARSE, "Packet %s has an invalid item stack count (%u)", full_path, (unsigned)stack_count);
		goto fail;
	}
	if(!bytes_remaining(f, &remaining)){goto fail;}
	if(stack_count > remaining / (2 * sizeof(uint32_t))){
		LOG(PARSE, "Packet %s claims %u item stacks but is too short to hold them", full_path, (unsigned)stack_count);
		goto fail;
	}
	if(stack_count > 0){
		o.inv.item_stack = XCALLOC(stack_count, sizeof(struct ItemStack));
		o.inv.item_stack_count = (int16_t)stack_count;
		for(int i = 0; i < o.inv.item_stack_count; i++){
			if(!read_i32(f, &o.inv.item_stack[i].uid)){goto fail;}
			if(!read_i32(f, &o.inv.item_stack[i].count)){goto fail;}
		}
	}

	fclose(f);
	*c = o;
	return true;

fail:
	LOG(READ, "Failed reading packet %s", full_path);
	fclose(f);
	free_save_component(&o);
	return false;
}
