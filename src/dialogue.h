#pragma once
#include <stdbool.h>

#define MAX_TOKEN_VALUE 256
#define MAX_TEXT_LENGTH 256
#define MAX_ENTRY_NAME 64
#define MAX_CONDITION_LENGTH 128
#define MAX_NODES_PER_ENTRY 32
#define MAX_ENTRIES 128
#define MAX_TOKENS 2048
#define TOKEN_LIST\
	X(TNARRATOR)\
	X(TBEGIN)\
	X(TREPLY)\
	X(TGOTO)\
	X(TEXIT)\
	X(TSET)\
	X(TSAY)\
	X(TAND)\
	X(TEND)\
	X(TOR)\
	X(TIF)\
	X(TIDENTIFIER)\
	X(TSTRING)\
	X(TMARK_EOF)
#define NODE_LIST\
	X(NODE_SAY)\
	X(NODE_REPLY)\
	X(NODE_SET_FLAG)
enum TokenType{
	#define X(name) name,
	TOKEN_LIST
	#undef X
	TOKEN_COUNT
};
enum NodeType{
	#define X(name) name,
	NODE_LIST
	#undef X
};

struct Token{
	enum TokenType type;
	char value[MAX_TOKEN_VALUE];
};
struct DialogueNode{
	enum NodeType type;
	char text[MAX_TEXT_LENGTH];
	char goto_entry[MAX_ENTRY_NAME];
	char condition[MAX_CONDITION_LENGTH];
	char set_flag[MAX_ENTRY_NAME];
	bool set_flag_value;
	bool is_narrator;
	bool is_exit;
};
struct DialogueEntry{
	struct DialogueNode nodes[MAX_NODES_PER_ENTRY];
	char name[MAX_ENTRY_NAME];
	char condition[MAX_CONDITION_LENGTH];
	int node_count;	
};
struct DialogueFile{
	struct DialogueEntry entries[MAX_ENTRIES];
	int entry_count;
};
struct Lexer{
	const char *src;
	int pos;
	struct Token tokens[MAX_TOKENS];
	int count;
};
struct Parser{
	struct Token *tokens;
	int pos;
	int count;
};
