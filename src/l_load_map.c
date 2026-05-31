#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "l_load_map.h"

bool l_write_map(const char *filepath, struct MapData *map){
	FILE *fp = fopen(filepath, "wb");
	if(!fp){return false;}	

	size_t size = sizeof(struct MapData) + map->width * map->height * sizeof(struct MapTileData);

	if(fwrite(map, size, 1, fp) != 1){fclose(fp); return false;}
	fclose(fp);
	return true;
}

struct MapData *l_read_map(const char *filepath){
	FILE *fp = fopen(filepath, "rb");
	if (!fp){return NULL;}

	struct MapData meta;
	if(fread(&meta, sizeof(struct MapData), 1, fp) != 1){
		fclose(fp);
		return NULL;
	}

	size_t size = sizeof(struct MapData) + meta.width * meta.height * sizeof(struct MapTileData);
	struct MapData *map = calloc(1, size);
	if(!map){fclose(fp);return NULL;}

	*map = meta;
	size_t tile_count = meta.width * meta.height;
	if(fread(map->data, sizeof(struct MapTileData), tile_count, fp) != tile_count){
		free(map);
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	return map;
}
