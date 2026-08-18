#include <stdbool.h>
#include <math.h>
#include "t_log_handler.h"
#include "t_math.h"

static bool src_chk(void *src);

v3 v3_cross(v3 a, v3 b){
	v3 result;
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
	return result;
}

vf3 vf3_cross(vf3 a, vf3 b){
	vf3 result;
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
	return result;
}

bool v2_is_nor(v2 *src){
    if(!src_chk(src)){return false;}
    double mag_sq = src->x * src->x + src->y * src->y;
    return fabs(mag_sq - 1.0) < 1e-9;
}
bool vf2_is_nor(vf2 *src){
    if(!src_chk(src)){return false;}
    float mag_sq = src->x * src->x + src->y * src->y;
    return fabsf(mag_sq - 1.0f) < 1e-6f;
}
bool v3_is_nor(v3 *src){
    if(!src_chk(src)){return false;}
    double mag_sq = src->x * src->x + src->y * src->y + src->z * src->z;
    return fabs(mag_sq - 1.0) < 1e-9;
}
bool vf3_is_nor(vf3 *src){
    if(!src_chk(src)){return false;}
    float mag_sq = src->x * src->x + src->y * src->y + src->z * src->z;
    return fabsf(mag_sq - 1.0f) < 1e-6f;
}

void v2_nor(v2 *src){
    if(!src_chk(src)){return;}
    double mag_sq = src->x * src->x + src->y * src->y;
    if(mag_sq == 0.0){return;}
    double inv_mag = 1.0 / sqrt(mag_sq);
    src->x *= inv_mag;
    src->y *= inv_mag;
}
void vf2_nor(vf2 *src){
    if(!src_chk(src)){return;}
    float mag_sq = src->x * src->x + src->y * src->y;
    if(mag_sq == 0.0f){return;}
    float inv_mag = 1.0f / sqrtf(mag_sq);
    src->x *= inv_mag;
    src->y *= inv_mag;
}
void v3_nor(v3 *src){
    if(!src_chk(src)){return;}
    double mag_sq = src->x * src->x + src->y * src->y + src->z * src->z;
    if(mag_sq == 0.0){return;}
    double inv_mag = 1.0 / sqrt(mag_sq);
    src->x *= inv_mag;
    src->y *= inv_mag;
    src->z *= inv_mag;
}
void vf3_nor(vf3 *src){
    if(!src_chk(src)){return;}
    float mag_sq = src->x * src->x + src->y * src->y + src->z * src->z;
    if(mag_sq == 0.0f){return;}
    float inv_mag = 1.0f / sqrtf(mag_sq);
    src->x *= inv_mag;
    src->y *= inv_mag;
    src->z *= inv_mag;
}

v2 v2_mke(int num){
	return (v2){num, num};
}
vf2 vf2_mke(float num){
	return (vf2){num, num};
}
v3 v3_mke(int num){
	return (v3){num, num};
}
vf3 vf3_mke(float num){
	return (vf3){num, num};
}

void v2_add(v2 *src, v2 add){
	if(!src_chk(src)){return;}
	src->x += add.x;
	src->y += add.y;
	return;
};
void v2_min(v2 *src, v2 tke){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x -= tke.x;
	src->y -= tke.y;
	return;
}
void v2_div(v2 *src, v2 div){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x /= div.x;
	src->y /= div.y;
	return;
}
void v2_mul(v2 *src, v2 mul){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x *= mul.x;
	src->y *= mul.y;
	return;
}

void vf2_add(vf2 *src, vf2 add){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x += add.x;
	src->y += add.y;
	return;
}
void vf2_min(vf2 *src, vf2 tke){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x -= tke.x;
	src->y -= tke.y;
	return;
}
void vf2_div(vf2 *src, vf2 div){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x /= div.x;
	src->y /= div.y;
	return;
}
void vf2_mul(vf2 *src, vf2 mul){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x *= mul.x;
	src->y *= mul.y;
	return;
}
 
void v3_add(v3 *src, v3 add){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x += add.x;
	src->y += add.y;
	src->z += add.z;
	return;
}
void v3_min(v3 *src, v3 tke){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x -= tke.x;
	src->y -= tke.y;
	src->z -= tke.z;
	return;
}
void v3_div(v3 *src, v3 div){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x /= div.x;
	src->y /= div.y;
	src->z /= div.z;
	return;
}
void v3_mul(v3 *src, v3 mul){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x *= mul.x;
	src->y *= mul.y;
	src->z *= mul.z;
	return;
}

void vf3_add(vf3 *src, vf3 add){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x += add.x;
	src->y += add.y;
	src->z += add.z;
	return;
}
void vf3_min(vf3 *src, vf3 tke){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x -= tke.x;
	src->y -= tke.y;
	src->z -= tke.z;
	return;
}
void vf3_div(vf3 *src, vf3 div){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x /= div.x;
	src->y /= div.y;
	src->z /= div.z;
	return;
}
void vf3_mul(vf3 *src, vf3 mul){
	if(!src){LOG(LOG_NULL, "Passed NULL src"); return;}
	src->x *= mul.x;
	src->y *= mul.y;
	src->z *= mul.z;
	return;
}

static bool src_chk(void *src){
	if(!src){
		LOG(LOG_NULL, "Passed NULL src"); 
		return false;
	}
	return true;
}

