#pragma once

#include <stdbool.h>

#define MAX_TOKEN_VALUE 256
#define MAX_NODES_PER_ENTRY 32
#define MAX_ENTRIES 128
#define MAX_ENTRY_NAME 64
#define MAX_CONDITION_LEN 128
#define MAX_TEXT_LEN 256
#define MAX_TOKENS 2048

enum TokenType{
	TOK_IF,
	TOK_BEGIN,
	TOK_END,
	TOK_SAY,
	TOK_REPLY,
	TOK_GOTO,
	TOK_EXIT,
	TOK_NARRATOR,
	TOK_SET,
	TOK_STRING,
	TOK_IDENTIFIER,
	TOK_AND,
	TOK_OR,
	TOK_EOF,
};

struct LookupDef{
	enum TokenType type;
	char *arg;
};

enum NodeType{
	NODE_SAY,
	NODE_REPLY,
	NODE_SET_FLAG,
};

struct Token{
	enum TokenType type;
	char	       value[MAX_TOKEN_VALUE];
};

struct DialogueNode{
	enum NodeType type;
	char text[MAX_TEXT_LEN];
	char goto_entry[MAX_ENTRY_NAME];
	char condition[MAX_CONDITION_LEN];
	char set_flag[MAX_ENTRY_NAME];
	bool set_flag_value;
	bool is_narrator;
	bool is_exit;
};
struct DialogueEntry{
	char name[MAX_ENTRY_NAME];
	char condition[MAX_CONDITION_LEN];
	struct DialogueNode node[MAX_NODES_PER_ENTRY];
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
