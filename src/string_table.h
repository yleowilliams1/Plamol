#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#define STR_NAME_SIZE 4

struct MessageHeader{
	uint64_t magic_number;
	uint32_t time_stamp;
	uint16_t endian_check;
	uint16_t version;
};
struct StringHook{
	uint32_t hash;
	char name[STR_NAME_SIZE];
	uint32_t offset;
};
struct MessageData{
	uint32_t blob_size;
	uint32_t blob_count;
	uint32_t table_count;
	char *blob;
	uint8_t *occupancy;
	struct StringHook *tbl;
};
// This is the format the text gets converted into
struct StringTable {
	char *str;
	char (*names)[STR_NAME_SIZE];
	uint32_t *offsets;
	uint32_t *sizes;
	size_t count;
	size_t str_size;
};

void write_message(const char *path, const char *out);
struct MessageData *read_message(const char *path);
void free_message(struct MessageData *msg);
uint32_t message_hash(const char *name);
const char *message_get(const struct MessageData *msg, const char *name);
const char *message_get_hash(const struct MessageData *msg, uint32_t hash);
struct StringTable *create_string_table();
const char *grab_string(struct StringTable *tbl, int gen);
void pop_str(struct StringTable *tbl, int gen);
int stack_str(struct StringTable *tbl, char *str, size_t len);
bool load_message_file(struct StringTable *tbl, const char *path, int expected_count);
