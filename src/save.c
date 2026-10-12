#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include "util/util.h"
#include "save.h"
#include "flag.h"
#include "settings.h"
#define BUF_SIZE 512

#define MAGIC "BensonAZ"
#define VERSION 1

// Everything is little-endian: write_header and write_u* are fixed-order.
bool write_save(const struct SaveFile *sf, int save_slot){
	if(!sf){LOG(IS_NULL, "write_save called with NULL save"); return false;}
	if(!sf->fm){LOG(IS_NULL, "write_save called with a save that has no flag manager"); return false;}
	if(sf->obj_count > 0 && !sf->objs){
		LOG(IS_NULL, "Save has %" PRIu32 " object refs but no ref array", sf->obj_count);
		return false;
	}
	if(save_slot < 0){LOG(INDEX, "Save slot cannot be less than 0"); return false;}

	const char *base = STR(SAVE_PATH);
	if(!base){LOG(IS_NULL, "SAVE_PATH not set"); return false;}
	size_t len = strlen(base);
	const char *sep = (len > 0 && base[len - 1] == '/') ? "" : "/";

	char path[BUF_SIZE];
	if(!t_snprintf(path, sizeof(path), NULL, "%s%s%d.sav", base, sep, save_slot)){
		LOG(WRITE, "Save path too long for slot %d", save_slot);
		return false;
	}

	FILE *f = fopen(path, "wb");
	if(!f){
		int err = errno;
		LOG(NO_FILE, "Could not open %s for writing: %s", path, strerror(err));
		return false;
	}

	struct Header h = create_header(MAGIC, VERSION);
	if(!write_header(f, &h)){goto fail;}
	if(!write_u32(f, sf->current_map_gindex)){goto fail;}

	if(!write_u32(f, sf->obj_count)){goto fail;}
	for(uint32_t i = 0; i < sf->obj_count; i++){
		if(!write_u32(f, sf->objs[i].uid)){goto fail;}
		if(!write_u32(f, sf->objs[i].slot)){goto fail;}
	}

	if(!write_u32(f, sf->fm->count)){goto fail;}
	for(uint32_t i = 0; i < sf->fm->count; i++){
		char name[MAX_FLAG_NAME] = {0};
		strncpy(name, sf->fm->flags[i].name, MAX_FLAG_NAME - 1);
		if(fwrite(name, 1, MAX_FLAG_NAME, f) != MAX_FLAG_NAME){goto fail;}
		if(!write_u8(f, sf->fm->flags[i].value)){goto fail;}
	}

	if(fclose(f) != 0){f = NULL; goto fail;}
	return true;

fail:
	LOG(WRITE, "Failed writing %s", path);
	if(f){fclose(f);}
	return false;
}
