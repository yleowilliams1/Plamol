#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "settings.h"
#include "util/util.h"
#include "string_table.h"

static bool gen_valid(struct StringTable *str, int gen, struct GUIDs *out);
static bool tble_valid(struct StringTable *str);
struct StringTable *create_string_table(){
	struct StringTable *tbl = XCALLOC(1, sizeof(struct StringTable));
	
	char *null_str = STR(NULL_STRING_REF);
	size_t null_str_size = strlen(null_str) + 1; // include null terminator

	tbl->arr = XCALLOC(1, null_str_size);
	memcpy(tbl->arr, null_str, null_str_size);
	tbl->arr_size = null_str_size;
	
	tbl->offset = XCALLOC(1, sizeof(size_t));
	tbl->offset[0] = 0;
	tbl->string_count = 1;	

	tbl->valid_gens = XCALLOC(1, sizeof(struct GUIDs));
	*(tbl->valid_gens) = (struct GUIDs){0};
	tbl->gen_count = 0;
	tbl->valid_gen_count = 1;
	
	return tbl;
}
void free_string_table(struct StringTable *tbl){
	if(!tbl){LOG(IS_NULL, "Table is NULL");return;}	
	
	if(tbl->arr){free(tbl->arr);}
	if(tbl->offset){free(tbl->offset);}
	if(tbl->valid_gens){free(tbl->valid_gens);}
	free(tbl);
}
// This is a bit ridiculous but it's fine I guess. this just pops manually. I should probably be storing strlen on stacking, just get rid of the raw size_t array of offsets wit ha struct with size_t offset and size_t length. Strlen is probably the biggest cost here, besides from the allocations.
void pop_str(struct StringTable *tbl, int gen){
	struct GUIDs id;
	if(!tble_valid(tbl)){LOG(IS_NULL, "string is NULL");return;}
	if(!gen_valid(tbl, gen, &id)){LOG(IS_NULL, "%d doesn't exist", gen);return;}
	
	size_t arr_start = tbl->offset[id.slot];
	size_t pop_size = strlen(tbl->arr + arr_start) + 1;   // '\0'
	size_t arr_end = arr_start + pop_size;	

	size_t new_arr_size = tbl->arr_size - pop_size;
	size_t new_offset_size = (tbl->string_count - 1) * sizeof *tbl->offset;
	size_t new_valid_gens_size = (tbl->valid_gen_count - 1) * sizeof *tbl->valid_gens;	
	
	// This can't actually do XCALLOC(1, 0) because gen_valid skips 0. The offset is always larger than 0
	char *new_arr = XCALLOC(1, new_arr_size);
	size_t *new_offset = XCALLOC(1, new_offset_size);
	struct GUIDs *new_valid_gens = XCALLOC(1, new_valid_gens_size);

	memcpy(new_arr, tbl->arr, arr_start);
	memcpy(new_arr + arr_start, tbl->arr + arr_end, tbl->arr_size - arr_end);
	tbl->arr_size = new_arr_size;	
		
	size_t fn = tbl->string_count;
	size_t fs = id.slot;

	memcpy(new_offset, tbl->offset, fs * sizeof(*tbl->offset));
	memcpy(new_offset + fs, tbl->offset + fs + 1, (fn - fs - 1) * sizeof(*tbl->offset));	
	
	size_t vn = tbl->valid_gen_count;
	size_t vs = id.valid_slot;

	memcpy(new_valid_gens, tbl->valid_gens,vs * sizeof *tbl->valid_gens);
	memcpy(new_valid_gens + vs, tbl->valid_gens + vs + 1, (vn - vs - 1) * sizeof *tbl->valid_gens);
	
	tbl->valid_gen_count--;
	tbl->string_count--;
	
	free(tbl->arr);
	free(tbl->offset);
	free(tbl->valid_gens);

	tbl->arr = new_arr;
	tbl->offset = new_offset;
	tbl->valid_gens = new_valid_gens;	
	
	// Push back slots	
	for(size_t i = 0; i < tbl->valid_gen_count; i++){
		if(tbl->valid_gens[i].slot > id.slot){tbl->valid_gens[i].slot--;}
		if(tbl->valid_gens[i].valid_slot > id.valid_slot){tbl->valid_gens[i].valid_slot--;}
	}
	for(size_t i = 0; i < tbl->string_count; i++){
		if (tbl->offset[i] > arr_start){tbl->offset[i] -= pop_size;}
	}
}
int stack_str(struct StringTable *tbl, char *str, size_t len){
	if(!str){LOG(IS_NULL, "string is NULL");return NULL_STR;}
	if(!tble_valid(tbl)){LOG(IS_NULL, "string is NULL");return NULL_STR;}	
	
	size_t size_size = sizeof(size_t);
	size_t guids_size = sizeof(struct GUIDs);

	size_t new_arr_size = tbl->arr_size + (len + 1);
	size_t new_offset_size = (tbl->string_count + 1) * size_size;
	size_t new_valgens_size = (tbl->valid_gen_count + 1) * guids_size; 

	char *new_arr = XMALLOC(new_arr_size);
	size_t *new_offset = XMALLOC(new_offset_size);
	struct GUIDs *new_valgens = XMALLOC(new_valgens_size);
	
	memcpy(new_arr, tbl->arr, tbl->arr_size);
	memcpy(new_arr + tbl->arr_size, str, len); 
	new_arr[new_arr_size - 1] = '\0';

	memcpy(new_offset, tbl->offset, new_offset_size - size_size);
	new_offset[tbl->string_count] = tbl->arr_size;	
	
	memcpy(new_valgens, tbl->valid_gens, new_valgens_size - guids_size);
	new_valgens[tbl->valid_gen_count] = (struct GUIDs){
		.gen = tbl->gen_count + 1,
		.slot = tbl->string_count,
		.valid_slot = tbl->valid_gen_count,
	};
	

	tbl->arr_size = new_arr_size;
	tbl->string_count++;
	tbl->gen_count++;
	tbl->valid_gen_count++;

	free(tbl->arr);
	free(tbl->offset);
	free(tbl->valid_gens);

	tbl->arr = new_arr;
	tbl->offset = new_offset;
	tbl->valid_gens = new_valgens;

	return tbl->valid_gen_count - 1;
}
static bool gen_valid(struct StringTable *str, int gen, struct GUIDs *out){
	if(!str){LOG(IS_NULL, "string is NULL");return false;}
	if(gen == 0){LOG(IS_NULL, "Tried to validate NULL generation. You cannot change string NULL."); return false;}
	for(int i = 0; i < str->valid_gen_count; i++){
		if(str->valid_gens[i].gen == gen){
			if(out){*out = str->valid_gens[i];}
			return true; 
		}
	}
	return false;
}
static bool tble_valid(struct StringTable *str){
	if(!str){LOG(IS_NULL, "string is NULL"); return false;}
	if(!str->arr){LOG(IS_NULL, "string array is NULL"); return false;}
	if(!str->offset){LOG(IS_NULL, "string sizes is NULL"); return false;}
	if(!str->valid_gens){LOG(IS_NULL, "No valid gens"); return false;}
	return true;
}

