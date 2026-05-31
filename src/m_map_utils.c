#include "m_map_utils.h"

int m_tile_to_indx(int x, int y, int width){
	return y * width + x;
}
