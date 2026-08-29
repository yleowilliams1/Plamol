#pragma once
#include "math.h"

#define WIN_PARAM \
	X(BY_X) \
	X(BY_Y) \
	X(BY_LARGEST)
enum WindowConvertParameter{
	#define X(id) id,
	WIN_PARAM
	#undef X
	WIN_PARAM_COUNT
};

struct Window{
	v2  res;
	vf2 scale;	
};

void update_window();
void init_window(char *name);
float wincvrt(float num, enum WindowConvertParameter param);
v2 wincvrtv2(v2 num);

void set_fullscreen();
void set_borderless();
