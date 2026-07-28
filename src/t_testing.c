#include <string.h>
#include <stdint.h>
#include "e_engine_settings.h"
#include "e_error_handler.h"
#include "f_flags.h"
#include "i_input.h"
#include "i_inventory.h"
#include "i_items.h"
#include "p_stats.h"
#include "m_map.h"
#include "m_map_tile.h"

#define FLAG_NAMES_SIZE 6
static const char *str_lokup[ENG_STR_COUNT] = { 
         [MAP_PATH] = "map_path",
         [TILE_PATH] = "tile_path",
         [PORTRAIT_PATH] = "portrait_path",
         [FLAG_PATH] = "flag_path",
         [ITEMS_PATH] = "items_path",
         [STATS_PATH] = "stats_path",
 [INVENTORY_PATH] = "inventory_path",
	[INPUT_PATH] = "input_path",
};
enum inventory_gindx{
	INV_ONE,
	INV_TWO,
	INV_THREE,
	INV_GINDX_COUNT,
};
enum item_gindx{
	ITEM_ONE,
	ITEM_TWO,
	ITEM_THREE,
	ITEM_GINDX_COUNT,
};
enum map_gindx{
	MAP_ONE,
	MAP_TWO,
	MAP_THREE,
	MAP_GINDX_COUNT,
};
enum tiles_gindx{
	TILE_ONE,
	TILE_TWO,
	TILE_THREE,
	TILE_GINDX_COUNT,
};
static struct Inventory inventory_expected[INV_GINDX_COUNT] = {
	[INV_ONE] = {
		.hotbar_items = {INV_ONE, INV_TWO, -1, -1, -1, -1, -1, -1},
		.inventory = {INV_THREE, INV_THREE, INV_THREE, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		              -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	},
	[INV_TWO] = {
		.hotbar_items = {INV_ONE, -1, -1, -1, -1, -1, -1, -1},
		.inventory = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		              -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	},
	[INV_THREE] = {
		.hotbar_items = {-1, -1, -1, -1, -1, -1, -1, -1},
		.inventory = {INV_THREE, INV_TWO, INV_THREE, INV_THREE, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		              -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	},
};
static const char *flag_names[FLAG_NAMES_SIZE] = {
	[0] = "flag_one",
	[1] = "flag_two",
	[2] = "flag_three",
	[3] = "flag_four",
	[4] = "flag_five",
	[5] = "flag_six",
};

static int flag_values[FLAG_NAMES_SIZE] = {
	[0] = 1,
	[1] = 0,
	[2] = 0,
	[3] = 1,
	[4] = 0,
	[5] = 1
};
 
void t_initalize_tests(){
	// Engine testing
	bool engine_succeded = e_load_engine_settings();
	if(!engine_succeded){
		ERR_LOG(ERR_FUCKED, "Failed to Load Engine Settings");	
	}
	
	char *engine_strings[ENG_STR_COUNT];
	for(int i = 0; i < ENG_STR_COUNT; i++){
		engine_strings[i] = e_grab_str(i);	
	}
	for(int i = 0; i < ENG_STR_COUNT; i++){
		ERR_LOG(ERR_OK, "Engine string %s = %s", str_lokup[i], engine_strings[i]);
	}
	
	// Flag testing
	f_init_flag();	
	for(int i = 0; i < FLAG_NAMES_SIZE; i++){
		int flag_val = (int)flag_get(flag_names[i]);
		enum ER_CODE code;
		if(flag_val == flag_values[i]){code = ERR_OK;}
		else{code = ERR_PARSE;}
		ERR_LOG(code, "Flag %s parsed value %d with expected value of %d", flag_names[i], flag_val, flag_values[i]);
	}		
	// Input testing
	// This doesn't do actual
	// button testing since this is just a static thing
	i_init_input();
	// Item Testing
	for(int i = 0; i < ITEM_GINDX_COUNT; i++){
		struct ItemDataSet add     = unpack(i_get_pckitemdata(i, S_ADD, true));
		struct ItemDataSet hit     = unpack(i_get_pckitemdata(i, S_HIT, true));
		struct ItemDataSet damage  = unpack(i_get_pckitemdata(i, S_DAMAGE, true));
		struct ItemDataSet consume = unpack(i_get_pckitemdata(i, S_CONSUME, true));

		uint32_t flags = i_get_pckitemdata(i, S_FLAG, true);
		int range = i_get_pckitemdata(i, S_RANGE, true);

		char *name = i_get_pckitemstrs(i, S_NAME, true);
		char *desc = i_get_pckitemstrs(i, S_DESCRIPTION, true);

		bool is_throwable   = (flags & (1 << FLAG_THROWABLE))   != 0;
		bool is_consumeable = (flags & (1 << FLAG_CONSUMEABLE)) != 0;

		char *mgs = "NAME: %s. DESC: %s. FLAGS: [throwable=%s, consumeable=%s] (raw=0x%X). "
			    "ADD(stat=%d,amt=%d) HIT(stat=%d,amt=%d) DMG(stat=%d,amt=%d) CONSUME(stat=%d,amt=%d) RANGE=%d";

		ERR_LOG(ERR_OK, mgs,
			name, desc,
			is_throwable ? "true" : "false",
			is_consumeable ? "true" : "false",
			flags,
			add.stat, add.amount,
			hit.stat, hit.amount,
			damage.stat, damage.amount,
			consume.stat, consume.amount,
			range);
	}		
	// Inventory Testing
	for(int i = 0; i < INV_GINDX_COUNT; i++){
		struct Inventory inv = i_get_inv_proto(i, true);
		enum ER_CODE code;
		if(memcmp(&inv, &inventory_expected[i], sizeof(struct Inventory)) == 0){
			code = ERR_OK;
		} else {
			code = ERR_PARSE;
		}
		ERR_LOG(code, "Inventory %d parsed and matched expected value: %s", i, code == ERR_OK ? "true" : "false");		
		struct DervBonusMatrix matrx = i_get_bonus_matrx(i, true);
		
		char matbuf[DERV_CAP * 8] = {0};
		int offset = 0;
		for(int d = 0; d < DERV_CAP; d++){
			offset += snprintf(matbuf + offset, sizeof(matbuf) - offset, "%d ", matrx.derv[d]);
		}

		ERR_LOG(ERR_OK, "Inventory %d bonus matrix: [ %s]", i, matbuf);
	}
	// Do tiles first
	for(int i = 0; i < MAP_GINDX_COUNT; i++){
		int tileset, seg_cap;
		int n_exit, s_exit, w_exit, e_exit;
		
		m_get_metadata(i, true, M_TILESET, &tileset);
		m_get_metadata(i, true, M_SEGMENT_CAP, &seg_cap);
		m_get_metadata(i, true, M_NORTH_EXIT, &n_exit);
		m_get_metadata(i, true, M_SOUTH_EXIT, &s_exit);
		m_get_metadata(i, true, M_WEST_EXIT, &w_exit);
		m_get_metadata(i, true, M_EAST_EXIT, &e_exit);
		
		ERR_LOG(ERR_OK, "Map [%d]: Tileset %d: SegmentSize %d: NorthExit %d: SouthExit %d: WestExit %d: EastExit %d:", i, tileset, seg_cap, n_exit, s_exit, w_exit, e_exit);
	}
	for(int i = 0; i < TILE_GINDX_COUNT; i++){
		int d[TILEINFO_COUNT];
		for(int j = 0; j < TILEINFO_COUNT; j++){
			d[j] = t_grab_tiledata(i, j, true);	
		}
		ERR_LOG(ERR_OK, "Tile [%d]: Portrait %d: Text %d: Combat %d: Loot %d: Combat Loot %d: Flag %d:", i, d[T_PORTRAIT], d[T_TEXT], d[T_COMBAT], d[T_LOOT], d[T_COMBAT_LOOT], d[T_FLAGS]);	
	}	
}
