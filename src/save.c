#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util/util.h"
#include "save.h"
#include "entity.h"
#include "settings.h"
#define PATH_MAX 512

#define PACKET_MAGIC "BensonAZ"
#define SAVE_MAGIC "BabyBlue"
#define PACKET_VERSION 1
#define PACKET_ENDIAN_CHECK 0x0102
// A little helper to not be casting everything
static int32_t swap_i32(int32_t v);
static void swap_entity_mutable(struct EntityMutable *m);
static void swap_packet_reference(struct PacketReference *r);
// Builds "base_path/packet_type_index" (caller frees). This is the folder
// a given entity type's packets live in -- format_path() then turns that
// into "base_path/packet_type_index/packet_item_index.SAVE".
static char *build_packet_dir(char *base_path, int packet_type_index);
struct SaveManager *create_save_manager(){
	struct SaveManager *save_manager = XCALLOC(1, sizeof(struct SaveManager));
	return save_manager;
}
int load_all_saves(struct SaveManager *save_manager, char *base_path){
	if(!save_manager){LOG(IS_NULL, "Passed NULL save_manager"); return 0;}
	if(!base_path){LOG(IS_NULL, "Passed NULL base_path"); return 0;}

	int loaded = 0;
	for(int i=0; i<SAVE_FILE_COUNT; i++){
		char *path = format_path(base_path, "SAVE", i);
		if(!path){continue;}

		// Most slots won't have a file yet on a fresh install - that's the
		// expected case, not worth read_save_file()'s NO_FILE/CRITICAL log,
		// so we check quietly first and only bother reading what's there.
		FILE *probe = fopen(path, "rb");
		if(!probe){free(path); continue;}
		fclose(probe);

		struct SaveFile *save = read_save_file(path);
		free(path);
		if(!save){continue;}

		if(save_manager->saves[i]){
			free(save_manager->saves[i]->refs);
			free(save_manager->saves[i]);
		}
		save_manager->saves[i] = save;
		loaded++;
	}
	return loaded;
}
void free_save_manager(struct SaveManager **save_manager){
	for(int i = 0; i < SAVE_FILE_COUNT; i++){
		if(!(*save_manager)->saves[i]){continue;}
		if(!(*save_manager)->saves[i]->refs){continue;}

		free((*save_manager)->saves[i]->refs); 
		(*save_manager)->saves[i]->refs = NULL;
	}	
	free(*save_manager);
	*save_manager = NULL;
}
bool get_mutable_from_save(struct SaveManager *save_manager, char *base_path, int save_file, int instance_id, struct EntityMutable *out){
	// Direct index into refs by instance_id -- O(1), no search.
	if(!save_manager){LOG(IS_NULL, "Passed NULL save_manager"); return false;}
	if(!base_path){LOG(IS_NULL, "Passed NULL base_path"); return false;}
	if(!out){LOG(IS_NULL, "Passed NULL out"); return false;}
	if(save_file < 0 || save_file >= SAVE_FILE_COUNT){LOG(INDEX, "save_file %d out of range", save_file); return false;}
	if(instance_id < 0 || instance_id >= INT(ENTITY_INSTANCE_COUNT)){LOG(INDEX, "instance_id %d out of range", instance_id); return false;}

	struct SaveFile *save = save_manager->saves[save_file];
	if(!save || !save->refs){return false;}

	struct PacketReference ref = save->refs[instance_id];

	char *dir = build_packet_dir(base_path, ref.packet_type_index);
	if(!dir){return false;}

	char *file_path = format_path(dir, "SAVE", ref.packet_item_index);
	free(dir);
	if(!file_path){return false;}

	struct SavePacket *packet = read_packet(file_path);
	free(file_path);
	if(!packet){return false;}

	*out = packet->m;
	free(packet);
	return true;
}
bool save_packet_to_file(struct SaveManager *save_manager, char *base_path, int save_file, int instance_id, struct EntityMutable entity){
	// Writes straight to the slot for instance_id -- no directory scan,
	// no growable array. Each instance always owns its own packet file,
	// named after its own instance_id.
	if(!save_manager){LOG(IS_NULL, "Passed NULL save_manager"); return false;}
	if(!base_path){LOG(IS_NULL, "Passed NULL base_path"); return false;}
	if(save_file < 0 || save_file >= SAVE_FILE_COUNT){LOG(INDEX, "save_file %d out of range", save_file); return false;}
	if(instance_id < 0 || instance_id >= INT(ENTITY_INSTANCE_COUNT)){LOG(INDEX, "instance_id %d out of range", instance_id); return false;}

	int packet_type_index = entity.mutable_gindex;

	char *dir = build_packet_dir(base_path, packet_type_index);
	if(!dir){return false;}

	char *file_path = format_path(dir, "SAVE", instance_id);
	free(dir);
	if(!file_path){
		LOG(IS_NULL, "Failed to format packet path for save_file %d", save_file);
		return false;
	}

	write_packet(entity, file_path);
	free(file_path);

	struct SaveFile *save = save_manager->saves[save_file];
	if(!save){
		save = XCALLOC(1, sizeof(struct SaveFile));
		if(!save){return false;}
		save->refs = XCALLOC(INT(ENTITY_INSTANCE_COUNT), sizeof(struct PacketReference));
		if(!save->refs){free(save); return false;}
		save_manager->saves[save_file] = save;
	}

	save->refs[instance_id].packet_type_index = packet_type_index;
	save->refs[instance_id].packet_item_index = instance_id;

	return true;
}
void write_save_file(struct SaveFile s, char *path){
	if(!path){LOG(IS_NULL, "Path is null so can't save"); return;}
	if(!dir_exists(path)){LOG(NO_FILE, "%s is not a valid directory", path);return;}
	if(!s.refs){LOG(IS_NULL,"Reference array is NULL for %s", path);return;}
	FILE *f = fopen(path, "wb");
	if(!f){LOG(NO_FILE, "Could not open %s for writing", path); return;}
	struct SaveFileHeader header = {0};
	memcpy(&header.magic_number, SAVE_MAGIC, sizeof(header.magic_number));
	header.time_stamp = (uint32_t)time(NULL);
	header.endian_check = PACKET_ENDIAN_CHECK;
	header.version = PACKET_VERSION;
	
	size_t instance_count = (size_t)INT(ENTITY_INSTANCE_COUNT);

	bool failed = false;
	if(!failed && fwrite(&header, sizeof(header), 1, f) != 1){failed = true;}
	if(!failed && fwrite(s.refs, sizeof(struct PacketReference), instance_count, f) != instance_count){failed = true;}
	if(!failed && fwrite(&s.current_map_gindex, sizeof(uint32_t), 1, f) != 1){failed = true;}
	fclose(f);
	if(failed){LOG(WRITE, "Failed to write packet to %s", path);}
	return;

}
struct SaveFile *read_save_file(char *path){
	if(!path){LOG(IS_NULL, "Path is NULL, can't read save file");return false;}

	FILE *f = fopen(path, "rb");
	if(!f){LOG(NO_FILE, "Could not open %s for reading", path);return false;}

	struct SaveFileHeader header = {0};
	if(fread(&header, sizeof(header), 1, f) != 1){
		LOG(READ, "Failed to read header from %s", path);
		fclose(f);
		return false;
	}

	if(memcmp(&header.magic_number, SAVE_MAGIC, sizeof(header.magic_number)) != 0){
		LOG(READ, "%s does not start with the expected magic number", path);
		fclose(f);
		return false;
	}

	bool need_swap = false;
	if(header.endian_check == PACKET_ENDIAN_CHECK){
		need_swap = false;
	} else if(header.endian_check == swap16(PACKET_ENDIAN_CHECK)){
		need_swap = true;
	} else {
		LOG(READ, "%s has a corrupt endian_check field", path);
		fclose(f);
		return false;
	}

	if(need_swap){
		header.time_stamp = swap32(header.time_stamp);
		header.version    = swap16(header.version);
	}

	if(header.version != PACKET_VERSION){
		LOG(READ, "%s has version %u, expected %u", path, header.version, PACKET_VERSION);
		fclose(f);
		return false;
	}

	size_t instance_count = (size_t)INT(ENTITY_INSTANCE_COUNT);

	struct SaveFile *save = XCALLOC(1, sizeof(struct SaveFile));
	save->refs = XCALLOC(instance_count, sizeof(struct PacketReference));
	if(fread(save->refs, sizeof(struct PacketReference), instance_count, f) != instance_count){
		LOG(READ, "Failed to read %zu references from %s", instance_count, path);
		free(save->refs);
		free(save);
		fclose(f);
		return false;
	}
	fclose(f);

	if(need_swap){
		for(size_t i=0; i<instance_count; i++){
			swap_packet_reference(&save->refs[i]);
		}
	}
	if(fread(&save->current_map_gindex, sizeof(uint32_t), 1, f) != 1){
		LOG(READ, "Failed to read %zu references from %s", instance_count, path);
		free(save->refs);
		free(save);
		fclose(f);
		return false;
	}
	if(need_swap){
		save->current_map_gindex = swap32(save->current_map_gindex);
	}
	return save;
}
void write_packet(struct EntityMutable m, char *path){
	if(!path){LOG(IS_NULL, "Path is NULL, can't write packet");return;}
	if(!dir_exists(path)){LOG(NO_FILE, "%s is not a valid directory", path);return;}

	FILE *f = fopen(path, "wb");
	if(!f){LOG(NO_FILE, "Could not open %s for writing", path);return;}
	struct SavePacket packet = {0};
	memcpy(&packet.magic_number, PACKET_MAGIC, sizeof(packet.magic_number));
	packet.time_stamp = (uint32_t)time(NULL);
	packet.endian_check = PACKET_ENDIAN_CHECK;
	packet.version = PACKET_VERSION;
	packet.m = m;

	bool failed = false;
	if(!failed && fwrite(&packet, sizeof(packet), 1, f) != 1){failed = true;}
	fclose(f);
	if(failed){LOG(WRITE, "Failed to write packet to %s", path);}
	return;
}
struct SavePacket *read_packet(char *path){
	if(!path){LOG(IS_NULL, "Path is NULL, can't read packet");return NULL;}
 
	FILE *f = fopen(path, "rb");
	if(!f){LOG(NO_FILE, "Could not open %s for reading", path);return NULL;}
 
	struct SavePacket *packet = XCALLOC(1, sizeof(struct SavePacket));;
	if(fread(packet, sizeof(packet), 1, f) != 1){
		LOG(READ, "Failed to read packet from %s", path);
		fclose(f);
		return NULL;
	}
	fclose(f);
 
	// magic_number was written via memcpy of the literal ASCII bytes,
	// so it's endian-agnostic; compare the raw bytes, not the integer.
	if(memcmp(&packet->magic_number, PACKET_MAGIC, sizeof(packet->magic_number)) != 0){
		LOG(READ, "%s does not start with the expected magic number", path);
		return NULL;
	}
 
	bool need_swap = false;
	if(packet->endian_check == PACKET_ENDIAN_CHECK){
		need_swap = false;
	} else if(packet->endian_check == swap16(PACKET_ENDIAN_CHECK)){
		need_swap = true;
	} else {
		LOG(READ, "%s has a corrupt endian_check field", path);
		return NULL;
	}
 
	if(need_swap){
		packet->time_stamp = swap32(packet->time_stamp);
		packet->version    = swap16(packet->version);
		swap_entity_mutable(&packet->m);
	}
 
	if(packet->version != PACKET_VERSION){
		LOG(READ, "%s has version %u, expected %u", path, packet->version, PACKET_VERSION);
		return NULL;
	}
 
	return packet;
}
static void swap_entity_mutable(struct EntityMutable *m){
	m->mutable_gindex   = swap_i32(m->mutable_gindex);
	m->immutable_gindex = swap_i32(m->immutable_gindex);

	m->current_direction = swap_i32(m->current_direction);

	for(int i=0; i<BASE_STAT_COUNT; i++){
		m->stats_modifier_array[i] = swap_i32(m->stats_modifier_array[i]);
	}

	m->current_health_points = swap_i32(m->current_health_points);
	m->current_action_points = swap_i32(m->current_action_points);

	for(int i=0; i<INVENTORY_SIZE; i++){
		m->inventory[i].item_gindex = swap_i32(m->inventory[i].item_gindex);
		m->inventory[i].count       = swap_i32(m->inventory[i].count);
		// filled is bool -> single byte, no swap needed
	}
	for(int i=0; i<HOTBAR_SIZE; i++){
		m->hotbar[i].item_gindex = swap_i32(m->hotbar[i].item_gindex);
		m->hotbar[i].count       = swap_i32(m->hotbar[i].count);
	}

	m->flags          = swap32(m->flags);
	m->dialogue_flags = swap32(m->dialogue_flags);

	m->position.x = swap_i32(m->position.x);
	m->position.y = swap_i32(m->position.y);
}
static void swap_packet_reference(struct PacketReference *r){
	r->packet_type_index = swap_i32(r->packet_type_index);
	r->packet_item_index = swap_i32(r->packet_item_index);
}
static char *build_packet_dir(char *base_path, int packet_type_index){
	size_t needed = 0;
	if(!t_snprintf(NULL, 0, &needed, "%s/%d", base_path, packet_type_index)){
		LOG(PARSE, "failed to compute packet directory length");
		return NULL;
	}
	char *dir = XMALLOC(needed + 1);
	if(!dir){return NULL;}
	if(!t_snprintf(dir, needed + 1, NULL, "%s/%d", base_path, packet_type_index)){
		free(dir);
		LOG(IS_NULL, "Failed to write packet directory string");
		return NULL;
	}
	return dir;
}

static int32_t swap_i32(int32_t v){
	uint32_t u = swap32((uint32_t)v);
	return (int32_t)u;
}
