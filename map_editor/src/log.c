#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include "log.h"
#define COLOR_RED    "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_GREEN  "\x1b[32m"
#define COLOR_RESET  "\x1b[0m"
static FILE *log_file = NULL;


/*Todo
 * Make this error thing protable to windows. Right now the posix functions will fail silently.
 * */
static enum LOG_LVL severity(enum LOG_CODE code){
	switch(code){
		case LOG_LOAD:	return LVL_FINE;
		case LOG_FREE:  return LVL_FINE;
		case LOG_SET:   return LVL_FINE;
		case LOG_GET:   return LVL_FINE;
		case LOG_ALLOC: return LVL_CRASH;
		case LOG_NO_FILE: return LVL_CRITICAL;
		case LOG_PARSE:   return LVL_CRITICAL;
		case LOG_INDX:    return LVL_WARNING;
		case LOG_NULL:    return LVL_WARNING;
		case LOG_RELOAD:  return LVL_WARNING;
		case LOG_ABORT: return LVL_CRASH;
		case LOG_OUTOFBOUNDS: return LVL_WARNING;
		default: return LVL_WARNING;
	}
}
static const char *sev_label(enum LOG_LVL lvl){
	switch(lvl){
		case LVL_FINE: return "FINE";
		case LVL_WARNING: return "WARNING";
		case LVL_CRITICAL: return "CRITICAL";
		case LVL_CRASH: return "CRASH";
		default: return "UNKOWN";
	}
}
static const char *code_label(enum LOG_CODE code){
	switch (code){
		case LOG_LOAD: return "LOAD";
		case LOG_FREE: return "FREE";
		case LOG_SET: return "SET";
		case LOG_GET: return "GET";
		case LOG_ALLOC: return "ALLOC";
		case LOG_NO_FILE: return "NO_FILE";
		case LOG_PARSE: return "PARSER";
		case LOG_INDX: return "INDX";
		case LOG_NULL: return "NULL";
		case LOG_RELOAD: return "RELOAD";
		case LOG_ABORT: return "ABORT";
		case LOG_OUTOFBOUNDS: return "OUT_OF_BOUNDS";
		default: return "UNKOWN";
	}
}
static const char *lvl_col(enum LOG_LVL lvl){
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

void log_func(enum LOG_CODE code, const char *file, int line, const char *func, const char *fmt, ...){
	char msg[512];
	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);
	
	enum LOG_LVL lvl = severity(code);
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
		log_func(LOG_INDX, file, line , func, "xmalloc called with size 0");
		return NULL;
	}
	void *p = malloc(size);
	if(!p){
		log_func(LOG_ALLOC, file, line, func, "malloc failed for %zu bytes", size);
	}
	return p;
}
void *xcalloc_impl(size_t count, size_t size, const char *file, int line, const char *func){
	if(size == 0){
		log_func(LOG_INDX, file, line, func, "xcalloc called with size 0");
		return NULL;
	}
	void *p = calloc(count, size);
	if(!p){
		log_func(LOG_ALLOC, file, line, func, "calloc failed for %zu x %zu bytes", count, size);
	}
	return p;
}
