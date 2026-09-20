#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "ui.h"
#include "util.h"
// Rule. You call void functions, no return jack shit
static struct UiElement **ui = NULL;
static size_t element_size = 0;

void init_ui(size_t elements){
	if(elements <= 0){LOG(IS_NULL, "%zu is not a valid UI element size", elements);return;}
	if(!ui){ui = XCALLOC(elements, sizeof(struct UilElement *)); element_size = elements; return;}	
	LOG(LOAD, "Can't reload UI.");
}
void free_ui(){
}
void new_impl(uint32_t param,int count, enum Type *types, ...){
	va_list ap;
	va_start(ap, types);
	
	for(int i = 0; i < count; i++){
		switch(types[i]){
			case T_INT: break;
			case T_LL: break;
			case T_DOUBLE: break;
			case T_STR: break;
		}
	}

	va_end(ap);
}
