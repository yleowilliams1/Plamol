#include <stdbool.h>
#include "t_gindex_tool.h"
#include "e_error_handler.h"
/*The job of this tool is to
 * manage between local index and global indexes.
 * Local index are generally arrays which store data in memory.
 * global indexes refer to loaded data, things like item prototypes 
 * index.*/
static bool valid(int size, int lindx){
	return (lindx < 0 || lindx >= size);
}

int t_find_free_lindx(struct local_indx *arr, int size){
	int lindx = NULL_INDX;
	for(int i = 0; i < size; i++){
		if(arr[i].active){continue;}
		lindx = i;
		break;
	}
	return lindx;
}
bool t_gset_lindx(struct local_indx *arr, int size, int gindx){
	int lindx = t_gindx_to_lindx(arr, size, gindx);
	if(lindx == NULL_INDX){return false;}
	arr[lindx].active = true;
	arr[lindx].gindx = gindx;
	return true;
}
bool t_lset_lindx(struct local_indx *arr, int size, int gindx, int lindx){
	if(valid(size, lindx)){return false;}
	arr[lindx].active = true;
	arr[lindx].gindx = gindx;
	return true;	
}
bool t_lfree_lindx(struct local_indx *arr, int size, int lindx){
	if(valid(size, lindx)){return false;}
	arr[lindx] = (struct local_indx){0};
	return true;
}
bool t_gfree_lindx(struct local_indx *arr, int size, int gindx){
	int lindx = t_gindx_to_lindx(arr, size, gindx);
	if(lindx == NULL_INDX){return false;}
	arr[lindx] = (struct local_indx){0};
	return true;
}
int t_gindx_to_lindx(struct local_indx *arr, int size, int gindx){
	int lindx = NULL_INDX;	
	for(int i = 0; i < size; i++){
		if(arr[i].gindx == gindx){
			lindx = i;
			break;
		}
	}
	return lindx;
}
int t_lindx_to_gindx(struct local_indx *arr, int size, int lindx){
	int gindx = NULL_INDX;
	if(valid(size, lindx)){
		return gindx;
	}
	return arr[lindx].gindx;
}
