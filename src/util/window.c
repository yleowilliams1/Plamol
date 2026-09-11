#include <raylib.h>
#include <time.h>
#include <stdint.h>
#include "util.h"
#include "pcg_basic.h"
#define BASE_X 1280
#define BASE_Y 720

pcg32_random_t rng;
void initalize_pran(){
	pcg32_srandom_r(&rng, time(NULL), (intptr_t)&rng);
}
uint32_t pran(int max_num){
	return pcg32_boundedrand_r(&rng, max_num + 1);
}
static struct Window window = {
		.res.x = BASE_X,
		.res.y = BASE_Y,
		.scale.x = 0.0f,
		.scale.y = 0.0f,
};
void set_fullscreen(){
	int monitor = GetCurrentMonitor();
	SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
	ToggleFullscreen();
}
void set_borderless(){
	ToggleBorderlessWindowed();
}
void init_window(char *name){
	InitWindow(window.res.x, window.res.y, name);
}	
void update_window(){
	if(!IsWindowResized()){return;}
    	window.scale.x = (float)GetScreenWidth()/window.res.x;
    	window.scale.y = (float)GetScreenHeight()/window.res.y;
}

float wincvrt(float num, enum WindowConvertParameter param){
	float result = num;
	
	float x = window.scale.x;
	float y = window.scale.y;
	
	switch(param){
		case BY_X:
			result *= x;
			break;
		case BY_Y:
			result *= y;
			break;
		case BY_LARGEST:			
			result = (x > y) ? num * y: num * x;
			break;
		default:
			result = num;
			break;

	}
	return result;
}

v2 wincvrtv2(v2 num){
	float x = window.scale.x;
	float y = window.scale.y;
	return (v2){num.x * x, num.y * y};
}
