#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
#include "string.h"
#include "util.h"
uint16_t swap16(uint16_t v){
	return (uint16_t)((v << 8) | (v >> 8));
}
uint32_t swap32(uint32_t v){
	return  (v << 24) |
		((v << 8)  & 0x00FF0000) |
		((v >> 8)  & 0x0000FF00) |
		(v >> 24);
}
uint64_t swap64(uint64_t v){
	return  (v << 56) |
		((v << 40) & 0x00FF000000000000ULL) |
		((v << 24) & 0x0000FF0000000000ULL) |
		((v << 8)  & 0x000000FF00000000ULL) |
		((v >> 8)  & 0x00000000FF000000ULL) |
		((v >> 24) & 0x0000000000FF0000ULL) |
		((v >> 40) & 0x000000000000FF00ULL) |
		(v >> 56);
}
char *t_strdup(const char *s){
	if(s == NULL){
		LOG(IS_NULL, "t_strdup called with NULL string");
		return NULL;
	;}
	
	// Null terminator
	size_t len = strlen(s) + 1;
	char *copy = XMALLOC(len);
	if(copy == NULL){return NULL;}

	memcpy(copy, s, len);
	return copy;
}

void t_cpy(char *val, char **out){
	if(out== NULL){
		LOG(IS_NULL, "t_cpy called with NULL destination pointer");
		return;
	}
	if(*out){
		free(*out);
		*out = NULL;
	}
	*out = t_strdup(val);
	if(*out == NULL && val != NULL){
		LOG(IS_NULL, "t_cpy failed to duplicate string due to NULL ptr returned from t_strdup");
	}
}
void t_atoi(const char *str, int *result){
	if(str == NULL || result == NULL){
		LOG(IS_NULL, "Called t_atoi with null str or null result");
		return;
	}
	while (isspace((unsigned char )*str)){
		str++;
	}
	if(*str == '\0'){
		LOG(PARSE, "string is empty");
		return;
	}
	
	int sign = 1;
	if(*str == '+' || *str == '-'){
		if(*str == '-') {sign = -1;}
		str++;
	}

	if(!isdigit((unsigned char)*str)){
		LOG(PARSE, "Invalid character in str");
		return;
	}
	long long value = 0;
	while (isdigit((unsigned char)*str)){
		int digit = *str - '0';
		value = value * 10 + digit;

		if(sign == 1 && value > INT_MAX){
			LOG(PARSE, "Overflow! Overflow! when converting!");
			return;
		}
		if(sign == -1 && -value < INT_MIN){
			LOG(PARSE, "Underflow! Underflow! when converting!");
			return;
		}
		str++;
	}

	while(isspace((unsigned char)*str)){
		str++;
	}
	if(*str != '\0'){
		LOG(PARSE, "Invalid character in str");
		return;
	}

	*result = (int)(sign * value);
	if(*result == NULL_ATOI){
		LOG(PARSE, "Holy shit, atoi successfully parsed. But the number is %d so unfortuntly it will have to return %d. Sorry the cool factor is signifantly more important than usablity. Just don't parse %d.", NULL_ATOI, NULL_ATOI + 1, NULL_ATOI);
		*result = *result + 1;
	}
	return;
}
void t_atof(const char *str, float *result){
	if(str == NULL || result == NULL){
		LOG(IS_NULL, "Called t_atof with null str or null result");
		return;
	}
	while (isspace((unsigned char)*str)){
		str++;
	}
	if(*str == '\0'){
		LOG(PARSE, "string is empty");
		return;
	}
 
	errno = 0;
	char *endptr = NULL;
	float value = strtof(str, &endptr);
 
	// strtof leaves endptr == str if it couldn't parse anything at all.
	if(endptr == str){
		LOG(PARSE, "Invalid character in str");
		return;
	}
	if(errno == ERANGE){
		if(value == HUGE_VALF || value == -HUGE_VALF){
			LOG(PARSE, "Overflow! Overflow! when converting!");
		} else {
			// value underflowed to +-0 or a subnormal
			LOG(PARSE, "Underflow! Underflow! when converting!");
		}
		return;
	}
 
	while(isspace((unsigned char)*endptr)){
		endptr++;
	}
	if(*endptr != '\0'){
		LOG(PARSE, "Invalid character in str");
		return;
	}
 
	*result = value;
	return;
}
static bool t_vsnprintf(char *buf, size_t bufsize, size_t *out_len, const char *fmt, va_list ap){
	if(!fmt){
		if(buf != NULL && bufsize > 0){buf[0] = '\0';}
		if(out_len) {*out_len = 0;}
		LOG(PARSE, "Tried to pass null format str which would be undefined behaviour. Returning empty string");
		return false;
	}
	if(bufsize > 0 && buf == NULL){
		LOG(PARSE, "Null buffer with buffer size set to %zu. What the fuck?", bufsize);
		return false;
	}
	
	int result = vsnprintf(buf, bufsize, fmt, ap);
	if(result < 0){
		if(buf != NULL && bufsize > 0){buf[0] = '\0';}
		if(out_len){*out_len = 0;}
		LOG(PARSE, "vsnprintf has failed. This could be due to a encoding error.");
		return false;
	}
	size_t would_write = (size_t)result;
	if(out_len) *out_len = would_write;

	if(bufsize > 0 && buf[bufsize - 1] != '\0' && would_write >= bufsize){
		buf[bufsize - 1] = '\0';
	}
	if(buf != NULL && bufsize > 0 && would_write >= bufsize){
        	LOG(PARSE, "The string has been truncated.");
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
char *xstrdup(const char *src){
	size_t len = strlen(src) + 1;
	char *dst = XCALLOC(len, 1);
	memcpy(dst, src, len);
	return dst;
}
bool write_u8(FILE *f, uint8_t v){
	return fwrite(&v, 1, 1, f) == 1;
}
bool write_u16(FILE *f, uint16_t v){
	uint8_t b[2] = {(uint8_t)(v & 0xFF), (uint8_t)(v >> 8)};
	return fwrite(b, 1, 2, f) == 2;
}
bool write_u32(FILE *f, uint32_t v){
	uint8_t b[4] = {
		(uint8_t)(v & 0xFF),
		(uint8_t)((v >> 8)  & 0xFF),
		(uint8_t)((v >> 16) & 0xFF),
		(uint8_t)((v >> 24) & 0xFF)
	};
	return fwrite(b, 1, 4, f) == 4;
}
bool write_bytes(FILE *f, const void *buf, size_t n){
	return n == 0 || fwrite(buf, 1, n, f) == n;
}

bool read_u8(FILE *f, uint8_t *out){
	if(!f || !out){return false;}
	return fread(out, 1, 1, f) == 1;
}
bool read_u16(FILE *f, uint16_t *out){
	uint8_t b[2];
	if(!f || !out){return false;}
	if(fread(b, 1, 2, f) != 2){return false;}
	*out = (uint16_t)(b[0] | (b[1] << 8));
	return true;
}
bool read_u32(FILE *f, uint32_t *out){
	uint8_t b[4];
	if(!f || !out){return false;}
	if(fread(b, 1, 4, f) != 4){return false;}
	*out = (uint32_t)b[0]
	     | ((uint32_t)b[1] << 8)
	     | ((uint32_t)b[2] << 16)
	     | ((uint32_t)b[3] << 24);
	return true;
}

bool read_i8(FILE *f, int8_t *out){
	uint8_t v;
	if(!read_u8(f, &v)){return false;}
	*out = (int8_t)v;
	return true;
}
bool read_i16(FILE *f, int16_t *out){
	uint16_t v;
	if(!read_u16(f, &v)){return false;}
	*out = (int16_t)v;
	return true;
}
bool read_i32(FILE *f, int32_t *out){
	uint32_t v;
	if(!read_u32(f, &v)){return false;}
	*out = (int32_t)v;
	return true;
}

bool read_bytes(FILE *f, void *buf, size_t n){
	if(!f || (n > 0 && !buf)){return false;}
	return n == 0 || fread(buf, 1, n, f) == n;
}
bool bytes_remaining(FILE *f, size_t *out){
	if(!f || !out){return false;}
	long here = ftell(f);
	if(here < 0){return false;}
	if(fseek(f, 0, SEEK_END) != 0){return false;}
	long end = ftell(f);
	if(fseek(f, here, SEEK_SET) != 0){return false;}
	if(end < here){return false;}
	*out = (size_t)(end - here);
	return true;
}


struct Header create_header(const char mag[8], uint16_t version){
	struct Header h = {0};
	memcpy(&h.magic, mag, sizeof(h.magic));
	h.time = (uint32_t)time(NULL);
	h.endian = HEADER_ENDIAN;
	h.version = version;
	return h;
}

bool write_header(FILE *f, const struct Header *h){
	// The magic is plain bytes, so it has no byte order to worry about.
	// h->endian is never written: every file is little-endian, so the field
	// only exists in memory.
	if(!write_bytes(f, &h->magic, sizeof(h->magic))){return false;}
	if(!write_u32(f, h->time)){return false;}
	if(!write_u16(f, h->version)){return false;}
	return true;
}

bool check_header(const struct Header *h, const char magic[8], uint16_t version){
	if(!h || !magic){LOG(IS_NULL, "check_header called with NULL"); return false;}
	if(memcmp(&h->magic, magic, sizeof(h->magic)) != 0){
		LOG(PARSE, "Bad magic number (expected %.8s)", magic);
		return false;
	}
	if(h->version != version){
		LOG(PARSE, "Unsupported version %u (expected %u)", (unsigned)h->version, (unsigned)version);
		return false;
	}
	return true;
}

bool read_header(FILE *f, const char magic[8], uint16_t version, struct Header *out){
	if(!f || !magic || !out){LOG(IS_NULL, "read_header called with NULL"); return false;}
	struct Header h = {0};
	if(!read_bytes(f, &h.magic, sizeof(h.magic)) || !read_u32(f, &h.time) || !read_u16(f, &h.version)){
		LOG(READ, "File is too short to contain a header");
		return false;
	}
	h.endian = HEADER_ENDIAN;
	if(!check_header(&h, magic, version)){return false;}
	*out = h;
	return true;
}
