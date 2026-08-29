#include <stdio.h>
#include "state.h"
#include "log.h"

struct State *create_state(){
	struct State *state = XCALLOC(1, sizeof(struct State));
	return state;
}
void free_state(struct State *state){
}
void update_state(struct State *state, float delta){
}
void draw_state(struct State *state){
}
