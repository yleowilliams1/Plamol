#include <stdbool.h>
#include <string.h>
#include "t_gindex_tool.h"
#include "t_log_handler.h"
#include "t_config_tool.h"

/*The job of this tool is to
 * manage between local index and global indexes.
 * Local index are generally arrays which store data in memory.
 * global indexes refer to loaded data, things like item prototypes 
 * index.*/


// Not a fan of this design anymore frankly it's probably better to do a rewrite in the future going off the pool design with the refs and etc
bool t_indxvalid(int size, int lindx){
	return (lindx >= 0 && lindx < size);
}

int t_find_free_lindx(struct local_indx *arr, int size){
	int lindx = NULL_INDX;
	if(!arr){LOG(LOG_INDX, "Passed null arr"); return lindx;}
	if(size < 0){LOG(LOG_INDX, "passed size is less than zero"); return lindx;}
	for(int i = 0; i < size; i++){
		if(arr[i].active){continue;}
		lindx = i;
		break;
	}
	if(!t_indxvalid(size, lindx)){
		LOG(LOG_INDX, "Failed to find valid lindx");
		return NULL_INDX;
	}
	return lindx;
}

bool h_gchecker(struct local_indx *arr, int size, int gindx, int *out){
	if(!arr){LOG(LOG_INDX, "Passed null array"); return false;}
	if(size < 0){LOG(LOG_INDX, "Passed size less than zero"); return false;}
	if(!out){LOG(LOG_NULL, "Passed null out to checker"); return false;}
	int lindx = t_gindx_to_lindx(arr, size, gindx);
	if(!t_indxvalid(size, lindx)){LOG(LOG_INDX, "gindx conversion failed"); return false;}
	*out = lindx;
	return true;
}
bool h_lchecker(struct local_indx *arr, int size, int lindx){
	if(!arr){LOG(LOG_INDX, "Passed null array"); return false;}
	if(size < 0){LOG(LOG_INDX, "Passed size less than zero"); return false;}
	if(!t_indxvalid(size, lindx)){LOG(LOG_INDX, "gindx conversion failed"); return false;}
	return true;
}
bool t_gset_lindx(struct local_indx *arr, int size, int gindx){
	int lindx = NULL_INDX;
	if(!h_gchecker(arr, size, gindx, &lindx)){return false;}
	arr[lindx].active = true;
	arr[lindx].gindx = gindx;
	return true;
}
bool t_lset_lindx(struct local_indx *arr, int size, int gindx, int lindx){
	if(!h_lchecker(arr, size, lindx)){return false;}
	arr[lindx].active = true;
	arr[lindx].gindx = gindx;
	return true;	
}
bool t_lfree_lindx(struct local_indx *arr, int size, int lindx){
	if(!h_lchecker(arr, size, lindx)){return false;}
	arr[lindx] = (struct local_indx){0};
	return true;
}
bool t_gfree_lindx(struct local_indx *arr, int size, int gindx){
	int lindx = NULL_INDX;
	if(!h_gchecker(arr, size, gindx, &lindx)){return false;}
	arr[lindx] = (struct local_indx){0};
	return true;
}
int t_gindx_to_lindx(struct local_indx *arr, int size, int gindx){
	int lindx = NULL_INDX;
	if(size < 0){LOG(LOG_INDX, "Size is less than 0"); return NULL_INDX;}	
	for(int i = 0; i < size; i++){
		if(!arr){LOG(LOG_NULL, "Array is NULL! Can't convert!"); continue;}
		if(arr[i].gindx == gindx && arr[i].active){
			lindx = i;
			break;
		}
	}
	return lindx;
}
int t_lindx_to_gindx(struct local_indx *arr, int size, int lindx){
	int gindx = NULL_INDX;
	if(!t_indxvalid(size, lindx)){
		return gindx;
	}
	return arr[lindx].gindx;
}
