#include <raylib.h>
#include "w_window_manager.h"

#define BASE_X 1280
#define BASE_Y 720

const int w_base_res_x = BASE_X;
const int w_base_res_y = BASE_Y;

float w_relative_scale_x = 1.0f;
float w_relative_scale_y = 1.0f;

void w_update_relative_scale(){
    w_relative_scale_x = (float)GetScreenWidth()/w_base_res_x;
    w_relative_scale_y = (float)GetScreenHeight()/w_base_res_y;
}

// Window convert
float wcnf(float num){
	float x = w_relative_scale_x;
	float y = w_relative_scale_y;
	return (x > y) ? num * x: y;
}

float wcnfx(float num){
	float x = w_relative_scale_x;
	return num * x;
}
float wcnfy(float num){
	float y = w_relative_scale_y;
	return num * y;
}
int wcni(int num){
	float x = w_relative_scale_x;
	float y = w_relative_scale_y;
	return (int)((x > y) ? num * x: y);
}
int wcnix(int num){
	float x = w_relative_scale_x;
	return (int)(num * x);
}
int wcniy(int num){	
	float y = w_relative_scale_y;
	return (int)(num * y);
}
Vector2 wcnv(Vector2 num){
	float x = w_relative_scale_x;
	float y = w_relative_scale_y;
	return (Vector2){num.x * x, num.y * y};
}
