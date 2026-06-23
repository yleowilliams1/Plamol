#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "t_config_tool.h"

bool t_check(char *line, char *arg){
	bool b = (strcmp(line, arg) == 0);
	return b;	
}

bool t_config(void *ptr, char *path, void (*func)(struct config_pack, void *ptr)){
	struct config_pack pack = {0};
	FILE *f = fopen(path, "r");
	if(!f){return false;}
	while(fgets(pack.line, sizeof(pack.line), f)){
		if(pack.line[0] == '\n' || pack.line[0] == '#' || pack.line[0] == ';'){continue;}
		if(pack.line[0] == '['){sscanf(pack.line, "[%63[^]]]", pack.current_section); continue;}
		if(sscanf(pack.line, "%63[^=]=%127[^\n]", pack.key, pack.value) == 2){
			char *v = pack.value;
			while(*v == ' ' || *v == '\t'){v++;}
			func(pack, ptr);	
		}
	}
	fclose(f);
	return true;
}	
