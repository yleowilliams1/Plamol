#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "string.h"
#include "log.h"

bool check(char *line, char *arg){
	// These arguments could return valid strings
	// with no null terminatino and just silently fail.
	// Fix later
	if(line == NULL){
		LOG(LOG_PARSE, "Line argument was NULL!");
		return false;
	}
	if(arg == NULL){
		LOG(LOG_PARSE, "Arg argument was NULL");
		return false;
	}
	
	return strcmp(line, arg) == 0;
}

bool config(void *ptr, char *path, ConfigLoader func){
	struct config_pack pack = {0};
	
	// There isn't need to check if the ptr is NULL
	// since it's a valid use case since t_config
	// never deferences it just passes it
	if(!path){LOG(LOG_NULL, "Passed NULL path to config"); return false;}
	if(!func){LOG(LOG_NULL, "Passed NULL loader to config"); return false;}

	FILE *f = fopen(path, "r");
	if(!f){LOG(LOG_ABORT, "Failed to open path %s. Aborting", path);}
 
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
			
			func(pack, ptr);	
		}
	}
	fclose(f);
	return true;
}
char *format_path(char *base, char *format, int num){
	if(!base){LOG(LOG_NULL, "Passed NULL base string");}
	if(!format){LOG(LOG_NULL, "Passed NULL format string");}

	size_t base_len = strlen(base);
	bool needs_slash = (base_len == 0 || base[base_len - 1] != '/');
	
	size_t needed = 0;
	const char *fmt = needs_slash ? "%s/%u.%s" : "%s%u.%s";

	if(!t_snprintf(NULL, 0, &needed, fmt, base, (unsigned int)num, format)){
		LOG(LOG_PARSE, "failed to compute length");
		return NULL;
	}

	char *file = XMALLOC(needed + 1);
	
	if(!t_snprintf(file, needed + 1, NULL, fmt, base, (unsigned int)num, format)){
		free(file);
		LOG(LOG_NULL, "Failed to write buffer");
		return NULL;
	}
	
	return file;
}
