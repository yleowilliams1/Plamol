#pragma once

struct State{
};

struct State *create_state();
void free_state(struct State *state);
void update_state(struct State *state, float delta);
void draw_state(struct State *state);
