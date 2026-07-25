#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "l_asset_manager.h"
#include "t_gindex_tool.h"
#include "e_error_handler.h"
#include "t_config_tool.h"

int l_getter_checks(int gindx, bool autoload, int cap, struct local_indx *iman, Loader ldr){
	int lindx = NULL_INDX;
	if(autoload){
		lindx = t_gindx_to_lindx(iman, cap, gindx);
		if(t_indxvalid(cap, lindx)){
			ERR_LOG(ERR_NULL, "Can't autoload alread loaded gindx %d", gindx);
		}else{
			bool loaded = ldr(gindx);
			if(!loaded){ERR_LOG(ERR_FUCKED, "Failed to load gindx %d", gindx);}
			int lindx = t_gindx_to_lindx(iman, cap, gindx);
			if(!t_indxvalid(cap, lindx)){
				ERR_LOG(ERR_FUCKED, "Gindx was freed since loading %d", gindx);	
			}
		}
	}
	else{
		int lindx = t_gindx_to_lindx(iman, cap, gindx);
		if(!t_indxvalid(cap, lindx)){
			ERR_LOG(ERR_INDX, "Didn't preload %d! Rerunning autload", gindx);
			return l_getter_checks(gindx, true, cap, iman, ldr);
		}
	}

	return lindx;
}

bool l_load_asset(struct AssetLoadPackage pckg){
	// Find free local index in the array
	int lindx = t_find_free_lindx(pckg.index_manager, pckg.gindx);
	if(!t_indxvalid(pckg.arr_cap, lindx)){
		ERR_LOG(ERR_FUCKED, "Asset array at path %s is full", pckg.path);
	}
	// Don't zero out. It's impossible for a 
	// lindx slot to be found that isn't zeroed out
	
	// Compute the byte address of the lindx-th element
	void *slot = (char *)pckg.arr + (size_t)lindx * pckg.element_size;

	// Hand over parsing to t_config loader
	bool loaded = t_loader(pckg.gindx, pckg.index_manager, pckg.function, pckg.path, slot, lindx);
	// Free the from the index manager 
	bool set    = t_lset_lindx(pckg.index_manager, pckg.arr_cap, pckg.gindx, lindx);

	if(!loaded){
		ERR_LOG(ERR_FUCKED, "Failed to load asset of gindx %d at path %s", pckg.gindx, pckg.path);
	}
	if(!set){
		ERR_LOG(ERR_FUCKED, "Failed to set lindx for index manager");
	}

	return true;
}

bool t_free_asset(struct AssetFreePackage pckg){
	int lindx = t_gindx_to_lindx(pckg.index_manager, pckg.arr_cap, pckg.gindx);
	if(!t_indxvalid(pckg.arr_cap, lindx)){
		ERR_LOG(ERR_FUCKED, "Tried to double free gindx %d", pckg.gindx);
	}

	// Zero out here
	char *base = (char *)pckg.arr;
	memset(base + (size_t)lindx * pckg.element_size, 0, pckg.element_size);	
	// Free from index manager
	bool indx_manfreed = t_lfree_lindx(pckg.index_manager, pckg.arr_cap, lindx);
	
	if(!indx_manfreed){
		ERR_LOG(ERR_FUCKED, "This shouldn't be possible? Somehow lindx has changed between the first t_indxvalid call in this function and the second in t_lfree_lindxs?");
	}
	return true;
}

