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
	S_FLAG,
	S_RANGE,
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
	// pack stat and amount to uint16
	uint32_t dataset[IDATA_COUNT];
	char *strs[ISTR_COUNT];
};

bool i_load_item(int gindx);
bool i_free_item(int gindx);
uint32_t i_get_pckitemdata(int gindx, enum ItemData d, bool autoload);
char *i_get_pckitemstrs(int gindx, enum ItemStrings d, bool autoload);
uint32_t pack_dataset(uint16_t a, uint16_t b);
struct ItemDataSet unpack(uint32_t packed);
