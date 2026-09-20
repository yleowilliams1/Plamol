#pragma once
#include <stddef.h>
#include <stdint.h>

#define UI_FLAGS\
	X(PASSTHROUGH)\
	X(MOVEABLE)\
	X(DRAW)\
	X(INHERIT_COLOR)
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))


enum UiFlags{
	#define X(name) name,
	UI_FLAGS
	#undef X
};
typedef void (*Fn)(int count, ...);
struct Clr {uint8_t r, g, b, a;};

struct Ui{
	int x;
	int y;
	int width;
	int height;
	int id;
	uint32_t flags;
	struct Clr clr;
};
struct Container{
	struct Clr clr;
};
struct Content{
};
struct Button{

};
struct Text{

};
struct ScrollBar{

};
struct Node{
	void *parent;
	void *child;
};
struct UiElement{
	struct Ui root;
	struct Node *relations;
	size_t relation_size;
};
enum Type { T_INT, T_LL, T_DOUBLE, T_STR, T_FN};

void new_impl(uint32_t param, int count, enum Type *types, ...);

#define COUNT_ARGS(...) COUNT_ARGS_(__VA_ARGS__, 8,7,6,5,4,3,2,1,0)
#define COUNT_ARGS_(_1,_2,_3,_4,_5,_6,_7,_8,N,...) N

#define CAT(a,b) CAT_(a,b)
#define CAT_(a,b) a##b
#define FE_1(f,x)      f(x)
#define FE_2(f,x,...)  f(x), FE_1(f,__VA_ARGS__)
#define FE_3(f,x,...)  f(x), FE_2(f,__VA_ARGS__)
#define FE_4(f,x,...)  f(x), FE_3(f,__VA_ARGS__)
#define FE_5(f,x,...)  f(x), FE_4(f,__VA_ARGS__)
#define FE_6(f,x,...)  f(x), FE_5(f,__VA_ARGS__)
#define FE_7(f,x,...)  f(x), FE_6(f,__VA_ARGS__)
#define FE_8(f,x,...)  f(x), FE_7(f,__VA_ARGS__)
#define FOR_EACH(f,...) CAT(FE_, COUNT_ARGS(__VA_ARGS__))(f, __VA_ARGS__)
#define TAG(x) _Generic((x),          \
    int:         T_INT,               \
    long long:   T_LL,                \
    float:       T_DOUBLE,            \
    double:      T_DOUBLE,            \
    char *:      T_STR,               \
    const char*: T_STR		\
    Fn: T_FN)
#define new(...) new_impl(COUNT_ARGS(__VA_ARGS__),(enum Type[]){ FOR_EACH(TAG, __VA_ARGS__) }, __VA_ARGS__)
