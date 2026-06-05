#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "p_player.h"

// Just move by one tile
#define PLAYER_SPEED 1

static struct Player *player;

void p_init_player(int tx, int ty){
	player = malloc(sizeof(struct Player));

	player->x = tx;
	player->y = ty;	
}
void p_free(){
	free(player);
}

void p_move_x(bool can_move, int dir){
	if(!can_move){printf("Player: Can't move X\n"); return;}
	player->x = dir * PLAYER_SPEED;	
}
void p_move_y(bool can_move, int dir){
	if(!can_move){printf("Player: Can't move Y\n"); return;}
	player->y = dir * PLAYER_SPEED;
}
