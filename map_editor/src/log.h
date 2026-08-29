#pragma once
#include <stdio.h>
#include <stdlib.h>
enum LOG_LVL{
	LVL_FINE,
	LVL_WARNING,
	LVL_CRITICAL,
	LVL_CRASH,
};

enum LOG_CODE{
	LOG_LOAD,
	LOG_FREE,
	LOG_SET,
	LOG_GET,
	LOG_ALLOC,
	LOG_NO_FILE,
	LOG_PARSE,
	LOG_INDX,
	LOG_NULL,
	LOG_RELOAD,
	LOG_ABORT,
	LOG_OUTOFBOUNDS,
};
void log_init(const char *path);
void log_func(enum LOG_CODE code, const char *file, int line, const char *func, const char *fmt, ...);
void *xmalloc_impl(size_t size, const char *file, int lie, const char *func);
void *xcalloc_impl(size_t count, size_t size, const char *file, int line, const char *func);
#define LOG(code, fmt, ...) log_func(code, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XMALLOC(size) xmalloc_impl(size, __FILE__, __LINE__, __func__)
#define XCALLOC(count, size) xcalloc_impl(count, size, __FILE__, __LINE__, __func__)
