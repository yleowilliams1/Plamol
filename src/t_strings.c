#include <stdlib.h>
#include <string.h>
#include "t_strings.h"
#include "e_error_handler.h"

char *t_strdup(const char *s){
	if(s == NULL){
		ERR_LOG(ERR_NULL, "t_strdup called with NULL string");
		return NULL;
	;}
	
	// Null terminator
	size_t len = strlen(s) + 1;
	char *copy = XMALLOC(len);
	if(copy == NULL){return NULL;}

	memcpy(copy, s, len);
	return copy;
}

void t_cpy(char **str, char *val){
	if(str == NULL){
		ERR_LOG(ERR_NULL, "t_cpy called with NULL destination pointer");
		return;
	}
	if(*str){
		free(*str);
		*str = NULL;
	}
	*str = t_strdup(val);
	if(*str == NULL && val != NULL){
		ERR_LOG(ERR_NULL, "t_cpy failed to duplicate string due to NULL ptr returned from t_strdup");
	}
}
