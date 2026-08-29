#include <raylib.h>
#include "window.h"

#define BASE_X 1280
#define BASE_Y 720

static struct Window window = {
		.res.x = BASE_X,
		.res.y = BASE_Y,
		.scale.x = 0.0f,
		.scale.y = 0.0f,
};
void init_window(char *name){
	InitWindow(window.res.x, window.res.y, name);
}	
void update_window(){
	if(!IsWindowResized()){return;}
    	window.scale.x = (float)GetScreenWidth()/window.res.x;
    	window.scale.y = (float)GetScreenHeight()/window.res.x;
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
