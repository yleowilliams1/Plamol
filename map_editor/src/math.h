#pragma once
#include <stdbool.h>

typedef struct v2{
	int x;
	int y;	
}v2;

typedef struct v3{
	int x;
	int y;
	int z;
}v3;

typedef struct vf2{
	float x;
	float y;
}vf2;

typedef struct vf3{
	float x;
	float y;
	float z;
}vf3;

v3 v3_cross(v3 a, v3 b);
vf3 vf3_cross(vf3 a, vf3 b);

bool v2_is_nor(v2 *src);
bool vf2_is_nor(vf2 *src);
bool v3_is_nor(v3 *src);
bool vf3_is_nor(vf3 *src);

void v2_nor(v2 *src);
void vf2_nor(vf2 *src);
void v3_nor(v3 *src);
void vf3_nor(vf3 *src);

v2 v2_mke(int num);
vf2 vf2_mke(float num);
v3 v3_mke(int num);
vf3 vf3_mke(float num);

void v2_add(v2 *src, v2 add);
void v2_min(v2 *src, v2 tke);
void v2_div(v2 *src, v2 div);
void v2_mul(v2 *src, v2 mul);

void vf2_add(vf2 *src, vf2 add);
void vf2_min(vf2 *src, vf2 tke);
void vf2_div(vf2 *src, vf2 div);
void vf2_mul(vf2 *src, vf2 mul);

void v3_add(v3 *src, v3 add);
void v3_min(v3 *src, v3 tke);
void v3_div(v3 *src, v3 div);
void v3_mul(v3 *src, v3 mul);

void vf3_add(vf3 *src, vf3 add);
void vf3_min(vf3 *src, vf3 tke);
void vf3_div(vf3 *src, vf3 div);
void vf3_mul(vf3 *src, vf3 mul);


