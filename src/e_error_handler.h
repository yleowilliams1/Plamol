#pragma once
#include <stdio.h>
#include <stdlib.h>
enum ER_LVL{
	LVL_WARNING,
	LVL_CRITICAL,
	LVL_CRASH,
};

enum ER_CODE{
	ERR_OK,
	ERR_ALLOC,
	ERR_NO_FILE,
	ERR_PARSE,
	ERR_INDX,
	ERR_NULL,
	ERR_RELOAD,
	ERR_FUCKED,
	ERR_OUTOFBOUNDS,
};
void err_init(const char *path);
void err_log(enum ER_CODE code, const char *file, int line, const char *func, const char *fmt, ...);
void *xmalloc_impl(size_t size, const char *file, int lie, const char *func);
void *xcalloc_impl(size_t count, size_t size, const char *file, int line, const char *func);
#define ERR_LOG(code, fmt, ...) err_log(code, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XMALLOC(size) xmalloc_impl(size, __FILE__, __LINE__, __func__)
#define XCALLOC(count, size) xcalloc_impl(count, size, __FILE__, __LINE__, __func__)
