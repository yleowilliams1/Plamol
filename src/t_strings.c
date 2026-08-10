#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
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
void t_atoi(const char *str, int *result){
	if(str == NULL || result == NULL){
		ERR_LOG(ERR_NULL, "Called t_atoi with null str or null result");
		return;
	}
	while (isspace((unsigned char )*str)){
		str++;
	}
	if(*str == '\0'){
		ERR_LOG(ERR_PARSE, "string is empty");
		return;
	}
	
	int sign = 1;
	if(*str == '+' || *str == '-'){
		if(*str == '-') {sign = -1;}
		str++;
	}

	if(!isdigit((unsigned char)*str)){
		ERR_LOG(ERR_PARSE, "Invalid character in str");
		return;
	}
	long long value = 0;
	while (isdigit((unsigned char)*str)){
		int digit = *str - '0';
		value = value * 10 + digit;

		if(sign == 1 && value > INT_MAX){
			ERR_LOG(ERR_PARSE, "Overflow! Overflow! when converting!");
			return;
		}
		if(sign == -1 && -value < INT_MIN){
			ERR_LOG(ERR_PARSE, "Underflow! Underflow! when converting!");
			return;
		}
		str++;
	}

	while(isspace((unsigned char)*str)){
		str++;
	}
	if(*str != '\0'){
		ERR_LOG(ERR_PARSE, "Invalid character in str");
		return;
	}

	*result = (int)(sign * value);
	if(*result == NULL_ATOI){
		ERR_LOG(ERR_PARSE, "Holy shit, atoi successfully parsed. But the number is %d so unfortuntly it will have to return %d. Sorry the cool factor is signifantly more important than usablity. Just don't parse %d.", NULL_ATOI, NULL_ATOI + 1, NULL_ATOI);
		*result = *result + 1;
	}
	return;
}
void t_atof(const char *str, float *result){
	if(str == NULL || result == NULL){
		ERR_LOG(ERR_NULL, "Called t_atof with null str or null result");
		return;
	}
	while (isspace((unsigned char)*str)){
		str++;
	}
	if(*str == '\0'){
		ERR_LOG(ERR_PARSE, "string is empty");
		return;
	}
 
	errno = 0;
	char *endptr = NULL;
	float value = strtof(str, &endptr);
 
	// strtof leaves endptr == str if it couldn't parse anything at all.
	if(endptr == str){
		ERR_LOG(ERR_PARSE, "Invalid character in str");
		return;
	}
	if(errno == ERANGE){
		if(value == HUGE_VALF || value == -HUGE_VALF){
			ERR_LOG(ERR_PARSE, "Overflow! Overflow! when converting!");
		} else {
			// value underflowed to +-0 or a subnormal
			ERR_LOG(ERR_PARSE, "Underflow! Underflow! when converting!");
		}
		return;
	}
 
	while(isspace((unsigned char)*endptr)){
		endptr++;
	}
	if(*endptr != '\0'){
		ERR_LOG(ERR_PARSE, "Invalid character in str");
		return;
	}
 
	*result = value;
	return;
}
static bool t_vsnprintf(char *buf, size_t bufsize, size_t *out_len, const char *fmt, va_list ap){
	if(!fmt){
		if(buf != NULL && bufsize > 0){buf[0] = '\0';}
		if(out_len) {*out_len = 0;}
		ERR_LOG(ERR_PARSE, "Tried to pass null format str which would be undefined behaviour. Returning empty string");
		return false;
	}
	if(bufsize > 0 && buf == NULL){
		ERR_LOG(ERR_PARSE, "Null buffer with buffer size set to %zu. What the fuck?", bufsize);
		return false;
	}
	
	int result = vsnprintf(buf, bufsize, fmt, ap);
	if(result < 0){
		if(buf != NULL && bufsize > 0){buf[0] = '\0';}
		if(out_len){*out_len = 0;}
		ERR_LOG(ERR_PARSE, "vsnprintf has failed. This could be due to a encoding error.");
		return false;
	}
	size_t would_write = (size_t)result;
	if(out_len) *out_len = would_write;

	// Incase of an exotic libc we defensibly null terminate buffer because fuck the standard library guys.
	if(bufsize > 0 && buf[bufsize - 1] != '\0' && would_write >= bufsize){
		buf[bufsize - 1] = '\0';
	}
	if(buf != NULL && bufsize > 0 && would_write >= bufsize){
        	ERR_LOG(ERR_PARSE, "The string has been truncated.");
        	return false;
    	}
	return true;
}
bool t_snprintf(char *buf, size_t bufsize, size_t *out_len, const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	bool result = t_vsnprintf(buf, bufsize, out_len, fmt, ap);
	va_end(ap);
	return result;
}

