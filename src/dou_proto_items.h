#pragma once
#include <stdbool.h>
#include <stdint.h>
enum ItemFlags{
	FLAG_THROWABLE,
	FLAG_CONSUMEABLE,
	FLAG_COUNT,
};

enum ItemData{
	S_ADD,
	S_HIT,
	S_DAMAGE,
	S_CONSUME,
	IDATA_COUNT,
};
enum ItemStrings{
	S_NAME,
	S_DESCRIPTION,
	ISTR_COUNT,
};

struct ItemDataSet{
	int stat;
	int amount;
};

struct Item{
	struct ItemDataSet dataset[IDATA_COUNT];
	char *strs[ISTR_COUNT];
	uint32_t flags;
	int range;
};

struct DouLoader;

struct DouLoader dou_item();
