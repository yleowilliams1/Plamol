#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "settings.h"
#include "util/util.h"
#include "string_table.h"

#define MAGIC "TheClash"
#define VERSION 1
#define ENDIAN 0x0102

static unsigned int round_p2(unsigned int v);
static uint32_t fnv1a(const char* data, size_t len);

static void free_string_table(struct StringTable *t);
static bool is_str(struct StringTable *str);
static struct StringTable *string_table_parse(const char *path);
static bool take(FILE *f, void *data, size_t size, size_t count);
static const char *find_slot(const struct MessageData *msg, uint32_t hash, const char *key);
static bool put(FILE *f, const void *data, size_t size, size_t count);
static void swap_hooks(struct StringHook *tbl, size_t count);

// fwrite of these structs is only portable while they have no padding.
_Static_assert(sizeof(struct MessageHeader) == 16, "MessageHeader has padding");
_Static_assert(sizeof(struct StringHook) == 12, "StringHook has padding");

void write_message(const char *path, const char *out){
	if(!path){LOG(IS_NULL, "Path is NULL can't write"); return;}
	if(!out){LOG(IS_NULL, "Out is NULL so can't write anywhere"); return;}

	// Validate before allocating anything, so there is nothing to leak on the early return.
	// is_str logs the specific reason, so no second log here.
	struct StringTable *str = string_table_parse(path);
	if(!is_str(str)){free_string_table(str); return;}

	struct MessageData *msg = XCALLOC(1, sizeof(struct MessageData));

	// manually copy from one to the other
	msg->blob_size = str->str_size;
	msg->blob_count = str->count;
	msg->table_count = round_p2((msg->blob_count * 4 + 2) / 3);
	if(msg->table_count == 0){msg->table_count = 1;}   // round_p2(0) is 0, which would make mask 0xFFFFFFFF

	msg->blob = XCALLOC(1, msg->blob_size ? msg->blob_size : 1);
	memcpy(msg->blob, str->str, msg->blob_size);
	msg->tbl = XCALLOC(1, sizeof(struct StringHook) * msg->table_count);
	msg->occupancy = XCALLOC(1, sizeof(uint8_t) * msg->table_count);

	uint32_t mask = msg->table_count - 1;

	// Look for duplicate strings. Duplicate names are already rejected by the parser.
	for (size_t a = 0; a < msg->blob_count; a++) {
		for (size_t b = a + 1; b < msg->blob_count; b++) {
			bool str_match = str->sizes[a] == str->sizes[b] && memcmp(str->str + str->offsets[a], str->str + str->offsets[b], str->sizes[a]) == 0;

			// Not intended behaviour, but not fatal either.
			if(str_match){LOG(PARSE, "Matched duplicate strings for '%.*s' and '%.*s': string: %s", STR_NAME_SIZE, str->names[a], STR_NAME_SIZE, str->names[b], str->str + str->offsets[a]);}
		}
	}

	// Hash here
	for(size_t i = 0; i < msg->blob_count; i++){
		uint32_t hash = fnv1a(str->names[i], strnlen(str->names[i], STR_NAME_SIZE));
		uint32_t idx = hash & mask;

		// Names are unique, so just probe to the next empty slot.
		while(msg->occupancy[idx]){
			// Same 32-bit hash for two different names: name lookup still works, hash-only lookup can't tell them apart.
			if(msg->tbl[idx].hash == hash){LOG(PARSE, "Hash collision between '%.*s' and '%.*s'", STR_NAME_SIZE, msg->tbl[idx].name, STR_NAME_SIZE, str->names[i]);}
			idx = (idx + 1) & mask;
		}

		// Write to table
		msg->occupancy[idx] = 1;
		msg->tbl[idx].hash = hash;
		memcpy(msg->tbl[idx].name, str->names[i], STR_NAME_SIZE);
		msg->tbl[idx].offset = str->offsets[i];
	}

	/*
	 * File layout:
	 *   MessageHeader                 (native byte order, endian_check tells the loader which)
	 *   blob_size, blob_count, table_count   (uint32_t each, native)
	 *   blob            (blob_size bytes)
	 *   occupancy       (table_count bytes)
	 *   StringHook[table_count]   native byte order
	 *   StringHook[table_count]   hash/offset byte-swapped
	 */
	bool ok = false;
	FILE *f = fopen(out, "wb");
	if(!f){LOG(NO_FILE, "Could not open %s for writing", out);}
	else{
		struct MessageHeader h = {0};
		memcpy(&h.magic_number, MAGIC, sizeof(h.magic_number));
		h.time_stamp = (uint32_t)time(NULL);
		h.endian_check = ENDIAN;
		h.version = VERSION;

		ok = put(f, &h, sizeof(h), 1)
		  && put(f, &msg->blob_size, sizeof(msg->blob_size), 1)
		  && put(f, &msg->blob_count, sizeof(msg->blob_count), 1)
		  && put(f, &msg->table_count, sizeof(msg->table_count), 1)
		  && put(f, msg->blob, 1, msg->blob_size)
		  && put(f, msg->occupancy, 1, msg->table_count)
		  && put(f, msg->tbl, sizeof(struct StringHook), msg->table_count);

		if(ok){
			// msg is freed below, so flipping in place is fine.
			swap_hooks(msg->tbl, msg->table_count);
			ok = put(f, msg->tbl, sizeof(struct StringHook), msg->table_count);
		}
		if(fclose(f) != 0){ok = false;}
		if(!ok){
			LOG(WRITE, "Failed to write %s", out);
			remove(out);   // don't leave a truncated file behind
		}
	}

	free_string_table(str);
	free_message(msg);
}
static bool put(FILE *f, const void *data, size_t size, size_t count){
	if(count == 0){return true;}   // fwrite returns 0 for an empty write, which is not an error
	return fwrite(data, size, count, f) == count;
}
static void swap_hooks(struct StringHook *tbl, size_t count){
	for(size_t i = 0; i < count; i++){
		tbl[i].hash   = swap32(tbl[i].hash);
		tbl[i].offset = swap32(tbl[i].offset);
		// name is raw bytes, nothing to swap
	}
}
/*
 * Reads a file produced by write_message. Returns NULL (and logs why) on any problem.
 * Free the result with free_message.
 */
struct MessageData *read_message(const char *path){
	if(!path){LOG(IS_NULL, "Path is NULL can't read"); return NULL;}

	FILE *f = fopen(path, "rb");
	if(!f){LOG(NO_FILE, "Could not open %s for reading", path); return NULL;}

	const char *why = NULL;
	struct MessageData *msg = NULL;
	struct MessageHeader h;
	uint32_t counts[3];
	bool foreign = false;
	long flen;
	uint64_t expect;
	size_t occupied = 0;

	if(fseek(f, 0, SEEK_END) != 0 || (flen = ftell(f)) < 0 || fseek(f, 0, SEEK_SET) != 0){why = "could not get file size"; goto fail;}

	if(!take(f, &h, sizeof(h), 1) || !take(f, counts, sizeof(counts), 1)){why = "file too short for header"; goto fail;}
	if(memcmp(&h.magic_number, MAGIC, sizeof(h.magic_number)) != 0){why = "bad magic number"; goto fail;}

	// endian_check was written in the writer's byte order, so it tells us whether to swap.
	if(h.endian_check != ENDIAN){
		if(h.endian_check != swap16((uint16_t)ENDIAN)){why = "bad endian marker"; goto fail;}
		foreign = true;
		h.version = swap16(h.version);
		for(int i = 0; i < 3; i++){counts[i] = swap32(counts[i]);}
	}
	if(h.version != VERSION){why = "unsupported version"; goto fail;}

	uint32_t blob_size = counts[0], blob_count = counts[1], table_count = counts[2];

	// find_slot relies on a power-of-two table that always has at least one empty slot.
	if(table_count == 0 || (table_count & (table_count - 1)) != 0){why = "table_count is not a power of two"; goto fail;}
	if(table_count <= blob_count){why = "table has no free slots"; goto fail;}

	// Check the sizes against the real file size before allocating anything.
	expect = sizeof(h) + sizeof(counts) + (uint64_t)blob_size + table_count + 2 * (uint64_t)table_count * sizeof(struct StringHook);
	if(expect != (uint64_t)flen){why = "file size does not match header counts"; goto fail;}

	msg = XCALLOC(1, sizeof(struct MessageData));
	msg->blob_size = blob_size;
	msg->blob_count = blob_count;
	msg->table_count = table_count;
	msg->blob = XCALLOC(1, blob_size ? blob_size : 1);
	msg->occupancy = XCALLOC(1, table_count);
	msg->tbl = XCALLOC(table_count, sizeof(struct StringHook));

	if(!take(f, msg->blob, 1, blob_size)){why = "short read on blob"; goto fail;}
	if(!take(f, msg->occupancy, 1, table_count)){why = "short read on occupancy"; goto fail;}

	// Two copies follow: the writer's native order, then swapped. Pick the one that matches this machine.
	if(foreign && fseek(f, (long)(table_count * sizeof(struct StringHook)), SEEK_CUR) != 0){why = "seek to second table failed"; goto fail;}
	if(!take(f, msg->tbl, sizeof(struct StringHook), table_count)){why = "short read on table"; goto fail;}

	// Don't trust the file: every occupied slot must point at a NUL-terminated string inside the blob.
	for(uint32_t i = 0; i < table_count; i++){
		if(msg->occupancy[i] > 1){why = "bad occupancy flag"; goto fail;}
		if(!msg->occupancy[i]){continue;}
		const struct StringHook *e = &msg->tbl[i];
		if(e->name[0] == '\0'){why = "empty name in table"; goto fail;}
		if(e->offset >= blob_size || !memchr(msg->blob + e->offset, '\0', blob_size - e->offset)){why = "string offset out of range"; goto fail;}
		if(e->hash != fnv1a(e->name, strnlen(e->name, STR_NAME_SIZE))){why = "hash does not match name"; goto fail;}
		occupied++;
	}
	if(occupied != blob_count){why = "occupied slots do not match blob_count"; goto fail;}

	fclose(f);
	return msg;

fail:
	LOG(READ, "Failed to read %s: %s", path, why);
	fclose(f);
	free_message(msg);
	return NULL;
}
// Hash for a 1-4 character name. Use this to get the hash for message_get_hash.
uint32_t message_hash(const char *name){
	if(!name){return 0;}
	return fnv1a(name, strnlen(name, STR_NAME_SIZE));
}
// Name -> string. Returns NULL if the name isn't in the table.
const char *message_get(const struct MessageData *msg, const char *name){
	if(!msg || !name){return NULL;}
	size_t len = strnlen(name, STR_NAME_SIZE + 1);
	if(len == 0 || len > STR_NAME_SIZE){return NULL;}   // can't exist, names are 1-NAME_SIZE chars
	char key[STR_NAME_SIZE] = {0};                      // zero-padded, same as stored names
	memcpy(key, name, len);
	return find_slot(msg, fnv1a(key, len), key);
}
// Hash -> string. If two names ever share a hash this returns whichever was inserted first (write_message logs that case).
const char *message_get_hash(const struct MessageData *msg, uint32_t hash){
	return find_slot(msg, hash, NULL);
}
// Linear probe, bounded by table_count so a full table can't loop forever. key may be NULL to match on hash alone.
static const char *find_slot(const struct MessageData *msg, uint32_t hash, const char *key){
	if(!msg || !msg->blob || !msg->tbl || !msg->occupancy || msg->table_count == 0){return NULL;}
	uint32_t mask = msg->table_count - 1;
	uint32_t idx = hash & mask;
	for(uint32_t n = 0; n < msg->table_count; n++){
		if(!msg->occupancy[idx]){return NULL;}   // empty slot ends the chain
		const struct StringHook *e = &msg->tbl[idx];
		if(e->hash == hash && (!key || memcmp(e->name, key, STR_NAME_SIZE) == 0)){
			return msg->blob + e->offset;
		}
		idx = (idx + 1) & mask;
	}
	return NULL;
}
static bool take(FILE *f, void *data, size_t size, size_t count){
	if(count == 0){return true;}
	return fread(data, size, count, f) == count;
}
static bool is_str(struct StringTable *str){
	if(!str){LOG(IS_NULL, "String table is NULL"); return false;}
	if(!str->str){LOG(IS_NULL, "String table string member is NULL"); return false;}
	if(!str->names){LOG(IS_NULL, "String table name is NULL"); return false;}
	if(!str->offsets){LOG(IS_NULL, "String table offsets is NULL"); return false;}
	if(!str->sizes){LOG(IS_NULL, "String table sizes is NULL"); return false;}
	return true;
}
void free_message(struct MessageData *msg){
	if(!msg){return;}
	free(msg->blob);
	free(msg->tbl);
	free(msg->occupancy);
	free(msg);
}
/*
 * Parses:
 *
 *   @NAME
 *   text, possibly many lines
 *
 *   @NAM2
 *   more text
 *
 */
static void free_string_table(struct StringTable *t){
	if(!t){return;}
	if(t->str){free(t->str); t->str = NULL;}
	if(t->names){free(t->names); t->names = NULL;}
	if(t->offsets){free(t->offsets); t->offsets = NULL;}
	if(t->sizes){free(t->sizes); t->sizes = NULL;}
	free(t);
}
static struct StringTable *string_table_parse(const char *path){
	if(!path){LOG(IS_NULL, "Path is NULL"); return NULL;}

	FILE *f = fopen(path, "rb");
	if(!f){LOG(IS_NULL, "Failed to open file to path %s", path);return NULL;}

	if(fseek(f, 0, SEEK_END) != 0){fclose(f); LOG(IS_NULL, "fseek to end failed for %s", path); return NULL;}
	long flen = ftell(f);
	if(flen < 0){fclose(f); LOG(IS_NULL, "ftell failed for %s", path); return NULL;}
	if(fseek(f, 0, SEEK_SET) != 0){fclose(f); LOG(IS_NULL, "fseek to start failed for %s", path); return NULL;}
	// Offsets and sizes are stored as uint32_t, so the file must fit in 32 bits.
	if((unsigned long)flen > UINT32_MAX){fclose(f); LOG(IS_NULL, "%s is too large (%ld bytes)", path, flen); return NULL;}
	size_t file_len = (size_t)flen;

	struct StringTable *t = XCALLOC(1, sizeof(struct StringTable));
	t->str = XCALLOC(1, file_len + 1);

	if(fread(t->str, 1, file_len, f) != file_len){free_string_table(t); fclose(f); LOG(IS_NULL, "Fread failed to match length for %s", path); return NULL;}
	fclose(f);

	t->str[file_len] = '\0';
	char *buf_end = t->str + strlen(t->str);

	size_t count = 0;
	for (char *p = t->str; *p; ) {
		if (*p == '@') count++;
		char *eol = strchr(p, '\n');
		p = eol ? eol + 1 : p + strlen(p);
	}

	size_t n = count ? count : 1;
	t->names = XCALLOC(1, STR_NAME_SIZE * n);
	t->offsets = XCALLOC(1, sizeof(uint32_t) * n);
	t->sizes = XCALLOC(1, sizeof(uint32_t) * n);

	char *w = t->str; // Set start word
	char *text = NULL;
	size_t i = 0;
	int line_no = 0;

	for (char *p = t->str; *p; ) {
		line_no++;
		char *eol  = strchr(p, '\n');
		char *next = eol ? eol + 1 : p + strlen(p);

		if (*p == '@') {
			if (text) {
				char *end = p;
				while (end > text && isspace((unsigned char)end[-1])) end--;
				size_t text_len = (size_t)(end - text);
				memmove(w, text, text_len);
				w[text_len] = '\0';
				t->offsets[i - 1] = (uint32_t)(w - t->str);
				t->sizes[i - 1]   = (uint32_t)text_len;
				w += text_len + 1;
			}

			char *name = p + 1;
			char *name_end = eol ? eol : name + strlen(name);
			while (name < name_end && isspace((unsigned char)*name)) name++;
			while (name_end > name && isspace((unsigned char)name_end[-1])) name_end--;
			size_t name_len = (size_t)(name_end - name);

			if (name_len == 0 || name_len > STR_NAME_SIZE) {
				LOG(IS_NULL, "%s:%d: name must be 1-%d chars", path, line_no, STR_NAME_SIZE);
				free_string_table(t);
				return NULL;
			}
			char key[STR_NAME_SIZE] = {0};   // zero-padded, matches how names are stored
			memcpy(key, name, name_len);
			for (size_t j = 0; j < i; j++) {
				if (memcmp(t->names[j], key, STR_NAME_SIZE) == 0) {
					LOG(IS_NULL, "%s:%d: duplicate name '%.*s'", path, line_no, (int)name_len, name);
					free_string_table(t);
					return NULL;
				}
			}
			memcpy(t->names[i], key, STR_NAME_SIZE);
			i++;
			text = next;
		}
		p = next;
	}
	if (text) {
		char *end = buf_end;
		while (end > text && isspace((unsigned char)end[-1])) end--;
		size_t text_len = (size_t)(end - text);
		memmove(w, text, text_len);
		w[text_len] = '\0';
		t->offsets[i - 1] = (uint32_t)(w - t->str);
		t->sizes[i - 1]   = (uint32_t)text_len;
		w += text_len + 1;
	}

	t->str_size = (size_t)(w - t->str);
	t->count = i;

	// Give back the slack: blob is allocated at file size but only str_size is used.
	char *shrunk = realloc(t->str, t->str_size ? t->str_size : 1);
	if (shrunk){ t->str = shrunk;}
	else{LOG(IS_NULL, "Realloc failed. My cowardice has cost us... :(");}
	return t;
}
static uint32_t fnv1a(const char* data, size_t len) {
	uint32_t hash = 2166136261;
	for (size_t i = 0; i < len; i++) {
		hash ^= (uint8_t)data[i];
		hash *= 16777619;
	}
	return hash;
}
static unsigned int round_p2(unsigned int v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}
