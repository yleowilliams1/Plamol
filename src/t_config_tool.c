#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "t_config_tool.h"
#include "t_gindex_tool.h"
#include "t_strings.h"
#include "e_error_handler.h"

bool t_check(char *line, char *arg){
	// These arguments could return valid strings
	// with no null terminatino and just silently fail.
	// Fix later
	if(line == NULL){
		ERR_LOG(ERR_PARSE, "Line argument was NULL!");
		return false;
	}
	if(arg == NULL){
		ERR_LOG(ERR_PARSE, "Arg argument was NULL");
		return false;
	}
	
	return strcmp(line, arg) == 0;
}

bool t_config(void *ptr, char *path, ParserType func){
	struct config_pack pack = {0};
	
	if(!path){ERR_LOG(ERR_FUCKED, "Passed NULL path to config");}
	
	FILE *f = fopen(path, "r");
	if(!f){ERR_LOG(ERR_FUCKED, "Failed to open path %s", path);}
 
	while(fgets(pack.line, sizeof(pack.line), f)){
		if(pack.line[0] == '\n' || pack.line[0] == '#' || pack.line[0] == ';'){continue;}
		if(pack.line[0] == '['){sscanf(pack.line, "[%63[^]]]", pack.current_section); continue;}
		if(sscanf(pack.line, "%63[^=]=%127[^\n]", pack.key, pack.value) == 2){
			// Trim key the same way value is trimmed below.
			// %63[^=] captures everything up to '=' literally,
			// including any space/tab before it (e.g. "flag_path " from "flag_path = x"),
			// which breaks every t_check() strcmp against a clean lookup string.
			char *k = pack.key;
			while(*k == ' ' || *k == '\t'){k++;}
			size_t klen = strlen(k);
			while(klen > 0 && (k[klen-1] == ' ' || k[klen-1] == '\t' || k[klen-1] == '\r')){
				klen--;
			}
			k[klen] = '\0';
			if(k != pack.key){
				memmove(pack.key, k, klen + 1);
			}
 
			char *v = pack.value;
	    		while(*v == ' ' || *v == '\t'){v++;}
	    		size_t len = strlen(v);
	    		while(len > 0 && (v[len-1] == ' ' || v[len-1] == '\t' || v[len-1] == '\r')){
				len--;
	    		}
	    	
			v[len] = '\0';
 
	   		if(v != pack.value){
				memmove(pack.value, v, len + 1); // +1 to include the null terminator
	    		}
	    		if(!func){ERR_LOG(ERR_FUCKED, "Passed NULL function ptr to config");}
			func(pack, ptr);	
		}
	}
	fclose(f);
	return true;
}
char *t_ini_plus_indx(char *base,int indx){
	if(!base){
		ERR_LOG(ERR_FUCKED, "Called with NULL base path");
	}
	size_t base_len = strlen(base);
	bool needs_slash = (base_len == 0 || base[base_len - 1] != '/');
	
	size_t needed = 0;
	const char *fmt = needs_slash ? "%s/%u.ini" : "%s%u.ini";

	if(!t_snprintf(NULL, 0, &needed, fmt, base, (unsigned int)indx)){
		ERR_LOG(ERR_FUCKED, "failed to compute length");
	}

	char *file = XMALLOC(needed + 1);
	if(!file){
		ERR_LOG(ERR_FUCKED, "No clue how this is being called. XMALLOC didn't crash and returned a NULL file?");
	}
	if(!t_snprintf(file, needed + 1, NULL, fmt, base, (unsigned int)indx)){
		free(file);
		ERR_LOG(ERR_FUCKED, "Failed to write buffer");
	}
	
	return file;
}
char *t_png_plus_indx(char *base,int indx){
	if(!base){
		ERR_LOG(ERR_FUCKED, "Called with NULL base path");
	}
	size_t base_len = strlen(base);
	bool needs_slash = (base_len == 0 || base[base_len - 1] != '/');
	
	size_t needed = 0;
	const char *fmt = needs_slash ? "%s/%u.png" : "%s%u.png";

	if(!t_snprintf(NULL, 0, &needed, fmt, base, (unsigned int)indx)){
		ERR_LOG(ERR_FUCKED, "failed to compute length");
	}

	char *file = XMALLOC(needed + 1);
	if(!file){
		ERR_LOG(ERR_FUCKED, "No clue how this is being called. XMALLOC didn't crash and returned a NULL file?");
	}
	if(!t_snprintf(file, needed + 1, NULL, fmt, base, (unsigned int)indx)){
		free(file);
		ERR_LOG(ERR_FUCKED, "Failed to write buffer");
	}
	
	return file;
}
bool t_loader(int gindx, struct local_indx *iman, ParserType func, char *path, void *ptr, int lindx){
	if(lindx == NULL_INDX){ERR_LOG(ERR_PARSE, "Passed NULL lindx"); return false;}
	// Can't be null
	char *file = t_ini_plus_indx(path, gindx);

	ERR_LOG(ERR_OK, "Loading %s", file);
	if(t_config(ptr, file, func)){
		free(file);
		return true;
	}
	ERR_LOG(ERR_PARSE, "Parse failed!");
	free(file);
	return false;
}	
