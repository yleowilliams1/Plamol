#pragma once

#define MAX_STATE_STACK 8

typedef void (*StateFunc)(void);


enum StateID{
    STATE_NONE,
    STATE_MAIN_MENU,
    STATE_EXPLORE,
    STATE_COUNT
};

struct GameStateNode{
    StateFunc on_enter;
    StateFunc on_exit;
    StateFunc on_update;
    StateFunc on_render;
    StateFunc on_pause;
    StateFunc on_resume;
};

struct StateMachine{
    struct GameStateNode stack[MAX_STATE_STACK];  
    int           count;                  
};

void sm_switch(struct GameStateNode node);
void sm_push(struct GameStateNode node);
void sm_pop(void);
void sm_update(void);
void sm_draw(void);
void sm_init(struct GameStateNode node); 
