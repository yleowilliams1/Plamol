#pragma once 
#include <stdbool.h>
#include <stdio.h>
#define NULL_ATOI 0x55AA
char *t_strdup(const char *s);
void t_cpy(char **str, char *val);
bool t_snprintf(char *buf, size_t bufsize, size_t *out_len, const char *fmt, ...);
void t_atoi(const char *str, int *result);
