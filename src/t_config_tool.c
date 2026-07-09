#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "t_config_tool.h"
#include "t_gindex_tool.h"
bool t_check(char *line, char *arg){
	bool b = (strcmp(line, arg) == 0);
	return b;	
}

bool t_config(void *ptr, char *path, ParserType func){
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

char *t_ini_plus_indx(char *base,int indx){
	size_t base_len = strlen(base);
	bool needs_slash = (base_len == 0 || base[base_len - 1] != '/');
	/*%s/%u.ini\0 base + slash + up to 10 digits + 4 + null*/
	char *file = malloc(base_len + needs_slash + 10 + 4 + 1);
	strcpy(file, base);
	if(needs_slash){file[base_len] = '/'; base_len++;}
	sprintf(file + base_len, "%u.ini", (unsigned int)indx);
	return file;
}
bool t_loader(int gindx, struct local_indx *iman, ParserType func, char *path, void *ptr, int lindx){
	if(lindx == NULL_INDX){return false;}
	char *file = t_ini_plus_indx(path, gindx);

	if(t_config(ptr, file, func)){
		free(file);
		return true;
	}
	free(file);
	return false;
}	
