#pragma once

#include "e_dou_manager.h"

struct Coordinator{
	struct DouManager *dous;
};

struct Coordinator *e_initalize_game();
void e_update_game(struct Coordinator *cor);
void e_draw_game(struct Coordinator *cor);
void e_free_game(struct Coordinator *cor);
