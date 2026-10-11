#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util/util.h"
#include "save.h"
#include "entity.h"
#include "settings.h"
#define PATH_MAX 512

#define PACKET_MAGIC "BensonAZ"
#define SAVE_MAGIC "BabyBlue"
#define PACKET_VERSION 1
#define PACKET_ENDIAN_CHECK 0x0102


if(!fm ){
LOG(IS_NULL, "write_flag called with NULL manager ");
return false;
}
if(fm->count > 0 && !fm->flags){
LOG(WRITE, "write_flag: count is %u but flags is NULL", fm->count);
return false;
}

bool ok = write_u32(f, fm->count);
for(uint32_t i = 0; ok && i < fm->count; i++){
char name[MAX_FLAG_NAME] = {0};
strncpy(name, fm->flags[i].name, MAX_FLAG_NAME - 1);

ok = fwrite(name, 1, MAX_FLAG_NAME, f) == MAX_FLAG_NAME
&& write_u8(f, fm->flags[i].value);
}

if(fclose(f) != 0){ok = false;}
if(!ok){LOG(WRITE, "write_flag failed writing");}
return ok;
