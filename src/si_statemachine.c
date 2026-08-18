#include <stdio.h> 
#include <stdbool.h>
#include "e_error_handler.h"
#include "g_statemachine.h"


// I should probably write error checking for this. But i don't want to.
// Most of this is fine. If a function is nulled then it's not really something to 
// announce

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
    	ERR_LOG(ERR_FUCKED, "Big nono. There shouldn't be more than that many states.");    
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
