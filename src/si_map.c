#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "si_map.h"

#include "t_config_tool.h"
#include "t_log_handler.h"
#include "t_strings.h"

#include "e_engine_settings.h"
#define VERSION 1
#define MAGIC_NUMBER 0x53414E444D414E00ULL
#define EXPECTED_ENDIAN 0xFFFE
#define SWAPPED_ENDIAN 0xFEFF

// This is basically done but it needs a assert and error checking doover like most of files right now so don't forget that once thigns are in place, write the asserts and descriptive log messages.

static bool si_validate_header(struct MapHeader *header);

void si_write_map_to_disk(struct Map *map, const char *filename){
	if(!map || !filename || !map->entity_instances || !map->interactable_instances || !map->ground_sprite_gindex || !map->wall_sprite_gindex || !map->ceiling_sprite_gindex){LOG(LOG_NULL, "Can't write something was NULL");return;}
	
	FILE *f = fopen(filename, "wb");
	if(!f){return;}
	struct MapHeader header = {0};	
	
	header.time_stamp = (uint32_t)time(NULL);
	header.magic_number = MAGIC_NUMBER;
	header.endian_check = 0xFFFE;
	header.version = VERSION;
	
	// write the header
	fwrite(&header, sizeof(struct MapHeader), 1, f);

	// Write the fixed size metadata array to file	
	fwrite(map->metadata, sizeof(int), M_META_COUNT, f);
	
	fwrite(map->entity_instances, sizeof(struct InstanceSlot), map->metadata[M_ENTITY_INSTANCE_COUNT], f);
	fwrite(map->interactable_instances, sizeof(struct InstanceSlot), map->metadata[M_INTERACTABLE_INSTANCE_COUNT], f);
	
	int tiles_size = map->metadata[M_WIDTH] * map->metadata[M_HEIGHT];
	
	fwrite(map->ground_sprite_gindex, sizeof(int), tiles_size, f);
	fwrite(map->wall_sprite_gindex, sizeof(int), tiles_size, f);
	fwrite(map->ceiling_sprite_gindex, sizeof(int), tiles_size, f);
	
	
	fclose(f);
}
struct Map *si_read_map_to_memory(const char *filename){
	if(!filename){LOG(LOG_NULL, "Can't read filename was NULL");return NULL;}	
	FILE *f = fopen(filename, "rb");
	if(!f){return NULL;}
	
	struct MapHeader header = {0};
	fread(&header, sizeof(struct MapHeader), 1, f);	
	if(!si_validate_header(&header)){fclose(f);return NULL;}

	struct Map *map = XCALLOC(1, sizeof(struct Map));
	fread(map->metadata, sizeof(int), M_META_COUNT, f);

	// Now that we have the metadata we can finish allocating the map
	map->entity_instances = XCALLOC(1, sizeof(struct InstanceSlot) * map->metadata[M_ENTITY_INSTANCE_COUNT]);	
	map->interactable_instances = XCALLOC(1, sizeof(struct InstanceSlot) * map->metadata[M_INTERACTABLE_INSTANCE_COUNT]);	
	int tiles_size = map->metadata[M_WIDTH] * map->metadata[M_HEIGHT];
	map->ground_sprite_gindex = XCALLOC(1, sizeof(int) * tiles_size);
	map->wall_sprite_gindex = XCALLOC(1, sizeof(int) * tiles_size);
	map->ceiling_sprite_gindex = XCALLOC(1, sizeof(int) * tiles_size);
	fread(map->entity_instances, sizeof(struct InstanceSlot), map->metadata[M_ENTITY_INSTANCE_COUNT], f);
	fread(map->interactable_instances, sizeof(struct InstanceSlot), map->metadata[M_INTERACTABLE_INSTANCE_COUNT], f);

	fread(map->ground_sprite_gindex, sizeof(int), tiles_size, f);
	fread(map->wall_sprite_gindex, sizeof(int), tiles_size, f);
	fread(map->ceiling_sprite_gindex, sizeof(int), tiles_size, f);

	fclose(f);
	return map;
}
// Btw none of this has saves. The saveing and loading happens on the instnatiation of the instances. On instantiation it checks against the save files and applys any change found in this map file
struct Map *si_load_map(int gindx){
	char *base = e_grab_sipath(ESI_MAP);
	char *path = t_format_path(base, ".map", gindx);
	return si_read_map_to_memory(path);
}
void si_free_map(struct Map *map){
	if(!map){return;}	
	if(map->entity_instances){free(map->entity_instances); map->entity_instances = NULL;}
	if(map->interactable_instances){free(map->interactable_instances); map->interactable_instances = NULL;}
	if(map->ground_sprite_gindex){free(map->ground_sprite_gindex); map->ground_sprite_gindex = NULL;}
	if(map->ceiling_sprite_gindex){free(map->ceiling_sprite_gindex); map->ceiling_sprite_gindex = NULL;}
	if(map->wall_sprite_gindex){free(map->wall_sprite_gindex); map->wall_sprite_gindex = NULL;}
	free(map);
	map = NULL;	
}
static bool si_validate_header(struct MapHeader *header){
	if(!header){LOG(LOG_NULL, "header was NULL");return false;}	
	if(header->magic_number != MAGIC_NUMBER){LOG(LOG_PARSE, "Magic number does not match"); return false;}
	if(header->endian_check == SWAPPED_ENDIAN){LOG(LOG_PARSE, "Endian mismatch ,need to byteswap");return false;}	
	if(header->endian_check != SWAPPED_ENDIAN && header->endian_check != EXPECTED_ENDIAN){LOG(LOG_NULL, "Header is corrupted");return false;}
	if(header->version != VERSION){LOG(LOG_OUTOFBOUNDS, "Possibly map won't be read since they are on differing version from compiled executable");}	
	return true;
}

