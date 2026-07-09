#pragma once
#include <stdbool.h>
#define NULL_INDX -1

struct local_indx{
	bool active;
	int gindx; 
};

int t_find_free_lindx(struct local_indx *arr, int size);
bool t_gset_lindx(struct local_indx *arr, int size, int gindx);
bool t_lset_lindx(struct local_indx *arr, int size, int gindx, int lindx);
bool t_lfree_lindx(struct local_indx *arr, int size, int lindx);
bool t_gfree_lindx(struct local_indx *arr, int size, int gindx);
int t_gindx_to_lindx(struct local_indx *arr, int size, int gindx);
int t_lindx_to_gindx(struct local_indx *arr, int size, int lindx);
