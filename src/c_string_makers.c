#include "c_stat_list.h"
#include "c_instance_list.h"
#include "c_depot_list.h"
#include "c_si_list.h"
const char *depstr(enum DepotType depot){
	switch(depot){
		#define X(id, type, fncs) case id: return #id;
		DEPOT_LIST
		#undef X
		case DEPOT_COUNT: return NULL;
		default: return NULL;
	}
}
const char *inststr(enum InstanceType type){
	switch(type){
		#define X(id) case id: return #id;
		INSTANCE_LIST
		#undef X
		case INSTANCE_COUNT: return NULL;
		default: return NULL;
	}
}
const char *bsttr(enum BaseStatEnum bstat){
	switch(bstat){
		#define X(name) case name: return #name;
		BSTAT_LIST
		#undef X
		default: return NULL;
	}
}
const char *dsttr(enum DerivedStatEnum dstat){
	switch(dstat){
		#define X(name) case name: return #name;
		DSTAT_LIST
		#undef X
		default: return NULL;
	}
}
const char *sistr(enum SiEnum si){
	switch(si){
		#define X(name) case name: return #name;
		SI_LIST
		#undef X
		default: return NULL;
	}
}
