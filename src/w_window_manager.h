#pragma once
#include <raylib.h>

extern const int w_base_res_x;
extern const int w_base_res_y;

extern float w_relative_scale_x;
extern float w_relative_scale_y;

void w_update_relative_scale();
float wcnf(float num);
float wcnfx(float num);
float wcnfy(float num);
int wcni(int num);
int wcnix(int num);
int wcniy(int num);
Vector2 wcnv(Vector2 num);
