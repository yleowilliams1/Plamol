#include <stdio.h> 
#include "g_statemachine.h"
/*
    NOTE: sm_submit_draw_calls doesn't actually render.
    All of the updates inside it are draw calls


*/
static struct StateMachine state_machine = {0};
void sm_init(struct GameStateNode node){
    sm_push(node);
} 
void sm_switch(struct GameStateNode node) {
    // pop current state then push new one
    // used for transitions like explore → combat
    // where you don't want to go back to explore when combat ends
    sm_pop();
    sm_push(node);
}

void sm_push(struct GameStateNode node) {
    if (state_machine.count >= MAX_STATE_STACK) {
        printf("ERROR: state stack overflow\n");
        return;
    }
    // push new state
    state_machine.stack[state_machine.count] = node;
    state_machine.count++;

    // enter new state
    if (node.on_enter) node.on_enter();
}

void sm_pop() {
    if (state_machine.count == 0) return;

    // exit current state
    if (state_machine.stack[state_machine.count - 1].on_exit) {
        state_machine.stack[state_machine.count - 1].on_exit();
    }
    state_machine.count--;
}

void sm_update() {
    if (state_machine.count == 0) return;
    
    if (state_machine.stack[state_machine.count - 1].on_update) {
        state_machine.stack[state_machine.count - 1].on_update();
    }
}

void sm_submit_draw_calls() {
    if (state_machine.count == 0) return;

    if (state_machine.stack[state_machine.count - 1].on_render) {
        state_machine.stack[state_machine.count - 1].on_render();
    }
}
