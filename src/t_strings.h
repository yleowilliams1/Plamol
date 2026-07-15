#pragma once 
#include <stdbool.h>
#include <stdio.h>

char *t_strdup(const char *s);
void h_cpy(char **str, char *val);
bool t_snprintf(char *buf, size_t bufsize, size_t *out_len, const char *fmt, ...);
void t_atoi(const char *str, int *result);
