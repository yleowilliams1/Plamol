#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util/util.h"
#include "save.h"
#include "entity.h"
#include "flag.h"
#include "settings.h"
#define BUF_SIZE 512 

#define MAGIC "BensonAZ"
#define VERSION 1

bool write_save(struct EntityManager *e, struct FlagManager *fm, int save_slot){
	if(!fm){LOG(IS_NULL, "write_flag called with NULL manager ");return false;}
	if(!e){LOG(IS_NULL, "entity manager is NULL"); return false;}
	if(save_slot < 0){LOG(IS_NULL, "SAve slot cannot be les than 0"); return false;}	
	
	const char *base = STR(SAVE_PATH);
	if(!base){LOG(IS_NULL, "SAVE_PATH not set"); return false;}

	char path[BUF_SIZE];
	if(!t_snprintf(path, sizeof(path), NULL, "%s%d.sav", base, save_slot)){
		LOG(WRITE, "Save path too long for slot %d", save_slot);
		return false;
	}	
	
	FILE *f = fopen(path, "wb");
	if(!f){LOG(NO_FILE, "write_object could not open %s", path);return false;}
	
	struct Header h = create_header(MAGIC, VERSION);
	if(fwrite(&h, sizeof(h), 1, f) != 1){fclose(f); return false;}
	
	// From here load the last_loaded_save, then loop through and copy it's objs and obj count, then loop through entity managewrs objects and take note whats there, and go back through the file and loop through checkign whats there and whats not, and writing/overridign depending on if it's the same.


	// Write flag	
	if(!write_u32(f, fm->count)){fclose(f); return false;}
	for(uint32_t i = 0; i < fm->count; i++){
		char name[MAX_FLAG_NAME] = {0};
		strncpy(name, fm->flags[i].name, MAX_FLAG_NAME - 1);

		if(fwrite(name, 1, MAX_FLAG_NAME, f) != MAX_FLAG_NAME){fclose(f); return false;}
		if(!write_u8(f, fm->flags[i].value)){fclose(f); return false;}
	}
	
	fclose(f);
	return true;
}
