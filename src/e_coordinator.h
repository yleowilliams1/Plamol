#pragma once

#include "e_dou_manager.h"
#include "si_flags.h"
#include "si_input.h"
#include "si_world.h"

struct Coordinator{
	struct DouManager *dous;
	struct FlagManager *flag_manager;
	struct InputManager *input;
	struct World *world;
};

struct Coordinator *e_initalize_game();
void e_update_game(struct Coordinator *cor);
void e_draw_game(struct Coordinator *cor);
void e_free_game(struct Coordinator *cor);
