#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "p_stats.h"
#include "p_entity.h"
#include "t_config_tool.h"
#define SPEED 1

// Write parser later

struct Entity *p_init_entity(int tx, int ty, int stat){
	return NULL;
}
void p_free(struct Entity *e){
	free(e);
}

void p_move_x(struct Entity *e ,bool can_move, int dir){
	if(!can_move){printf("Entity: Can't move X\n"); return;}
	e->x += dir * SPEED;	
}
void p_move_y(struct Entity *e, bool can_move, int dir){
	if(!can_move){printf("Entity: Can't move Y\n"); return;}
	e->y += dir * SPEED;
}
