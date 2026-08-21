#include <stdio.h> 
#include <stdbool.h>
#include "si_statemachine.h"

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

void sm_draw() {
    if (state_machine.count == 0) return;

    if (state_machine.stack[state_machine.count - 1].on_render) {
        state_machine.stack[state_machine.count - 1].on_render();
    }
}
void sm_free(){
	if(state_machine.count == 0) return;

	if(state_machine.stack[state_machine.count - 1].on_exit){
		state_machine.stack[state_machine.count - 1].on_exit();
	}
}
