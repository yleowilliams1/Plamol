#pragma once
#include <stdbool.h>
#include <stdint.h>

struct DialogueHeader{
	uint64_t magic_number;
	uint32_t time_stamp;
	uint16_t endian_check;
	uint16_t version;
};

// Design:
// Two windows. Node Window and Branch window. The node window is a large list of nodes, which can be edited.
// While the Branch window allows you link nodes to other nodes.

struct DialogueNode{
};
struct NodeManager{
	
};
