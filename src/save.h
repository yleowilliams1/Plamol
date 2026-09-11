#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "entity.h"
#define SAVE_FILE_COUNT 40
// Save files are made up of n save packets.
// Save packets are just data. We write
// both to binary blobs, but save file only holds
// some index to the data on disk not the actual
// data
struct SavePacket{
	uint64_t magic_number;
	uint32_t time_stamp;
	uint16_t endian_check;
	uint16_t version;
	struct EntityMutable m;		
};
// Heres how save files work. They store one big
// slot per possible entity instance -- refs is sized
// to INT(ENTITY_INSTANCE_COUNT) and indexed directly by
// instance_id, so finding a packet is a single array
// access, not a search. No count needs to be stored
// anywhere since the size always comes from settings.
// Each slot stores two things. First is type_index, which
// tells you which folder on disk the packet binary blob is,
// and then packet item_index is the index of the actual
// binary blob within the folder. Then save file is just a
// blob pointing to those files on disk. So thats why its as
// simple a format(path+packet_type_index, .PACK, packet_item_index) 
struct PacketReference{
	int packet_type_index;
	int packet_item_index;
};
struct SaveFileHeader{
	uint64_t magic_number;
	uint32_t time_stamp;
	uint16_t endian_check;
	uint16_t version;
};
struct SaveFile{
	// One slot per entity instance id, size INT(ENTITY_INSTANCE_COUNT).
	// refs[instance_id] is the reference for that instance.
	struct PacketReference *refs;
	uint32_t current_map_gindex;
};
struct SaveManager{
	struct SaveFile *saves[SAVE_FILE_COUNT];
};
struct SaveManager *create_save_manager();
void free_save_manager(struct SaveManager **save_manager);
int load_all_saves(struct SaveManager *save_manager, char *base_path);
bool get_mutable_from_save(struct SaveManager *save_manager, char *base_path, int save_file, int instance_id, struct EntityMutable *out);
bool save_packet_to_file(struct SaveManager *save_manager, char *base_path, int save_file, int instance_id, struct EntityMutable entity);
void write_save_file(struct SaveFile s, char *path);
struct SaveFile *read_save_file(char *path);
void write_packet(struct EntityMutable m, char *path);
struct SavePacket *read_packet(char *path);
