#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#define NULL_ATOI 0x55AA
#define COLOR_RED "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_GREEN "\x1b[32m"

#define TILE_W 64
#define TILE_H 32

#define LOG_LEVELS\
	X(OK, COLOR_GREEN)\
	X(WARNING, COLOR_YELLOW)\
	X(CRITICAL, COLOR_RED) \
	X(CRASH, COLOR_RED)
enum LOG_LVL{
	#define X(name, col) name,
	LOG_LEVELS
	#undef X
};
#define LOG_LIST\
	X(LOAD,    OK)\
	X(FREE,    OK)\
	X(SET,     OK)\
	X(GET,     OK)\
	X(ALLOC,   CRASH)\
	X(NO_FILE,  CRITICAL)\
	X(PARSE,   CRITICAL)\
	X(INDEX,   WARNING)\
	X(WRITE,   CRITICAL)\
	X(READ,    CRITICAL)\
	X(IS_NULL, WARNING)\
	X(ABORT,   CRASH)

enum LOG_CODE{
	#define X(name, lvl) name,
	LOG_LIST
	#undef X
};
struct config_pack{
	char line[256];
	char current_section[64];
	char key[64];
	char value[128];	
};
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

typedef void (*ConfigLoader)(struct config_pack, void *ptr);

char *format_path(char *base, char *format, int num);
bool check(char *line, char *arg);
bool config(void *ptr, char *path, ConfigLoader func);
void log_init(const char *path);
void log_func(enum LOG_CODE code, const char *file, int line, const char *func, const char *fmt, ...);
void *xmalloc_impl(size_t size, const char *file, int lie, const char *func);
void *xcalloc_impl(size_t count, size_t size, const char *file, int line, const char *func);
#define LOG(code, fmt, ...) log_func(code, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XMALLOC(size) xmalloc_impl(size, __FILE__, __LINE__, __func__)
#define XCALLOC(count, size) xcalloc_impl(count, size, __FILE__, __LINE__, __func__)
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
bool is_bit(int a, int b);
char *t_strdup(const char *s);
void t_cpy(char *val, char **out);
bool t_snprintf(char *buf, size_t bufsize, size_t *out_len, const char *fmt, ...);
void t_atoi(const char *val, int *out);
void t_atof(const char *val, float *out);
bool dir_exists(const char *path);

uint16_t swap16(uint16_t v);
uint32_t swap32(uint32_t v);
uint64_t swap64(uint64_t v);

void initalize_pran();
uint32_t pran(int max_num);

v2  world_to_tile(vf2 world);
vf2 tile_to_world(v2 tile);
