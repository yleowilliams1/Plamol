#include <stdlib.h>
#include <string.h>
#include "t_strings.h"

char *t_strdup(const char *s){
	if(s == NULL){return NULL;}
	
	// Null terminator
	size_t len = strlen(s) + 1;
	char *copy = malloc(len);
	if(copy == NULL){return NULL;}

	memcpy(copy, s, len);
	return copy;
}

void t_cpy(char *str, char *val){
	if(str){free(str);}
	str = t_strdup(val);
}
