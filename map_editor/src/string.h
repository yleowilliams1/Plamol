#pragma once 
#include <stdbool.h>
#include <stdio.h>
#define NULL_ATOI 0x55AA
char *t_strdup(const char *s);
void t_cpy(char *val, char **out);
bool t_snprintf(char *buf, size_t bufsize, size_t *out_len, const char *fmt, ...);
void t_atoi(const char *val, int *out);
void t_atof(const char *val, float *out);
