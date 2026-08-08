#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include "e_error_handler.h"
#define COLOR_RED    "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_GREEN  "\x1b[32m"
#define COLOR_RESET  "\x1b[0m"
static FILE *log_file = NULL;


/*Todo
 * Make this error thing protable to windows. Right now the posix functions will fail silently.
 * */
static enum ER_LVL severity(enum ER_CODE code){
	switch(code){
		case ERR_OK:	return LVL_FINE;
		case ERR_ALLOC: return LVL_CRASH;
		case ERR_NO_FILE: return LVL_CRITICAL;
		case ERR_PARSE:   return LVL_CRITICAL;
		case ERR_INDX:    return LVL_WARNING;
		case ERR_NULL:    return LVL_WARNING;
		case ERR_RELOAD:  return LVL_WARNING;
		case ERR_FUCKED: return LVL_CRASH;
		case ERR_OUTOFBOUNDS: return LVL_WARNING;
		default: return LVL_WARNING;
	}
}
static const char *sev_label(enum ER_LVL lvl){
	switch(lvl){
		case LVL_FINE: return "FINE";
		case LVL_WARNING: return "WARNING";
		case LVL_CRITICAL: return "CRITICAL";
		case LVL_CRASH: return "CRASH";
		default: return "UNKOWN";
	}
}
static const char *code_label(enum ER_CODE code){
	switch (code){
		case ERR_OK: return "OK";
		case ERR_ALLOC: return "ALLOC";
		case ERR_NO_FILE: return "NO_FILE";
		case ERR_PARSE: return "PARSER";
		case ERR_INDX: return "INDX";
		case ERR_NULL: return "NULL";
		case ERR_RELOAD: return "RELOAD";
		case ERR_FUCKED: return "FUCKED";
		case ERR_OUTOFBOUNDS: return "OUT_OF_BOUNDS";
		default: return "UNKOWN";
	}
}
static const char *lvl_col(enum ER_LVL lvl){
	switch(lvl){
		case LVL_FINE: return COLOR_GREEN;
		case LVL_WARNING: return COLOR_YELLOW;
		case LVL_CRITICAL: return COLOR_RED;
		case LVL_CRASH: return COLOR_RED;
		default : return COLOR_RESET;
	}
}
static void fatal_crash(void){
	fprintf(stderr, "%s[FATAL]%s A fatal error occurred. See error.log for details. \n", isatty(fileno(stderr)) ? COLOR_RED: "", 
	isatty(fileno(stderr)) ? COLOR_RESET : "");
	if(log_file){
		fprintf(log_file, "== FATAL: process aborting === \n");
		fflush(log_file);
		fclose(log_file);
		log_file = NULL;
	}

	abort();
}
void err_init(const char *path){
	log_file = fopen(path, "a");
	if(!log_file){
		printf("THE ERROR HANDLER LOG FILE FAILED\n");
	}
}

void err_log(enum ER_CODE code, const char *file, int line, const char *func, const char *fmt, ...){
	char msg[512];
	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);
	
	enum ER_LVL lvl = severity(code);
	const char *str_code = code_label(code);
	const char *str_lvl  = sev_label(lvl);

	time_t t = time(NULL);
	char timebuf[32];
	strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&t));
	
	// Terminal
	if(isatty(fileno(stderr))){
fprintf(stderr, "[%s:%s%s%s] %s (%s:%d in %s())\n", 
	str_code, lvl_col(lvl), str_lvl, COLOR_RESET, msg, file, line, func);	
	}
	else{
		fprintf(stderr, "[%s:%s] %s (%s)\n", str_code, str_lvl, msg, func);
	}
	// Write to file
	if(log_file){
		fprintf(log_file, "%s | %s:%s| code=%d | %s:%d in %s() | %s\n",
			timebuf, str_code, str_lvl, code, file, line, func,msg);
		fflush(log_file);
	}
	if(lvl == LVL_CRASH){
		fatal_crash();	
	}
}

// NO REALLOC. REALLOC IS FOR COWARDS.
void *xmalloc_impl(size_t size, const char *file, int line, const char *func){
	if(size == 0){
		err_log(ERR_INDX, file, line , func, "xmalloc called with size 0");
		return NULL;
	}
	void *p = malloc(size);
	if(!p){
		err_log(ERR_ALLOC, file, line, func, "malloc failed for %zu bytes", size);
	}
	return p;
}
void *xcalloc_impl(size_t count, size_t size, const char *file, int line, const char *func){
	if(size == 0){
		err_log(ERR_INDX, file, line, func, "xcalloc called with size 0");
		return NULL;
	}
	void *p = calloc(count, size);
	if(!p){
		err_log(ERR_ALLOC, file, line, func, "calloc failed for %zu x %zu bytes", count, size);
	}
	return p;
}
