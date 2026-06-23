#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "t_text.h" 
#include "p_entity.h"
#include "f_flags.h"
const struct LookupDef lookup[] = {
	{TOK_NARRATOR, "NARRATOR"},
	{TOK_BEGIN,    "BEGIN"},
	{TOK_REPLY,    "REPLY"},
	{TOK_GOTO,     "GOTO"},
	{TOK_EXIT,     "EXIT"},
	{TOK_SET,      "SET"},
	{TOK_SAY,      "SAY"},
	{TOK_AND,      "AND"},
	{TOK_END,      "END"},
	{TOK_OR,       "OR"},
	{TOK_IF,       "IF"},
};

void emit_token(int start, struct Lexer *lex, enum TokenType type){
	int len = lex->pos - start;
	if(len >= MAX_TOKEN_VALUE){len = MAX_TOKEN_VALUE - 1;}
	struct Token t = {0};
	t.type = type;
	strncpy(t.value, lex->src + start, len);
	t.value[len] = '\0';
	if(lex->count < MAX_TOKENS){lex->tokens[lex->count++] = t;}
}

bool is_ident_char(char c){
	return isalnum(c) || c == '_';
}

bool keyword_boundary(struct Lexer *lex, int offset){
	return !is_ident_char(lex->src[lex->pos + offset]);
}

struct Token *parser_peek(struct Parser *p){
	return &p->tokens[p->pos];
}
struct Token *parser_consume(struct Parser *p){
	return &p->tokens[p->pos++];
}
bool parser_match(struct Parser *p, enum TokenType type){
	if(p->tokens[p->pos].type == type){
		p->pos++;
		return true;
	}
	return false;
}

void lexer_scan(struct Lexer* lex){
	while(lex->src[lex->pos] != '\0'){
		while(lex->src[lex->pos] == ' '  ||
		      lex->src[lex->pos] == '\n' ||
		      lex->src[lex->pos] == '\r' ||
		      lex->src[lex->pos] == '\t'){
			lex->pos++;
		}

		if(lex->src[lex->pos] == '\0'){break;}

		// Skip comments
		if(lex->src[lex->pos] == '/' && lex->src[lex->pos + 1] == '/'){
			while(lex->src[lex->pos] != '\n' && lex->src[lex->pos] != '\0'){
				lex->pos++;
			}
			continue;
		}

		// String literals delimited by ~
		if(lex->src[lex->pos] == '~'){
			lex->pos++; 
			int start = lex->pos;
			while(lex->src[lex->pos] != '~' && lex->src[lex->pos] != '\0'){lex->pos++;}
			emit_token(start, lex, TOK_STRING);
			if(lex->src[lex->pos] == '~'){lex->pos++;}
			continue;
		}

		// Keyword lookup
		bool matched = false;
		for(int i = 0; i < sizeof(lookup) / sizeof(lookup[0]); i++){
			const struct LookupDef *v = &lookup[i];
			int len = strlen(v->arg);
			if(strncmp(lex->src + lex->pos, v->arg, len) == 0 && keyword_boundary(lex, len)){
				if(lex->count < MAX_TOKENS){lex->tokens[lex->count++] = (struct Token){v->type};}
				lex->pos += len;
				matched = true;
				break; 
			}
		}
		if(matched){continue;}

		if(isalpha(lex->src[lex->pos]) || lex->src[lex->pos] == '_'){
			int start = lex->pos;
			while(is_ident_char(lex->src[lex->pos])){lex->pos++;}
			emit_token(start, lex, TOK_IDENTIFIER);
			continue;
		}

		lex->pos++;
	}
	if(lex->count < MAX_TOKENS){
		lex->tokens[lex->count++] = (struct Token){TOK_EOF};
	}
}

void parse_entry(struct Parser *p, struct DialogueFile *file){
	if(file->entry_count >= MAX_ENTRIES){
		printf("[PRASER] ERROR: too many entries\n");
		return;
	}
	struct DialogueEntry *entry = &file->entries[file->entry_count++];
	memset(entry, 0, sizeof(struct DialogueEntry));

	if(parser_match(p, TOK_IF)){
		struct Token *condition = parser_consume(p);
		if(condition->type == TOK_STRING){
			strncpy(entry->condition, condition->value, MAX_CONDITION_LEN - 1);
		}
	}
	if(!parser_match(p, TOK_BEGIN)){
		printf("[PARSER] ERROR: expected BEGIN\n");
		return;
	}
	struct Token *name = parser_consume(p);
	if(name->type == TOK_IDENTIFIER){
		strncpy(entry->name, name->value, MAX_ENTRY_NAME - 1);
	}else{
		printf("[PARSER] ERROR: expected entry name after BEGIN\n");
		return;
	}
	while(parser_peek(p)->type != TOK_END &&
		parser_peek(p)->type != TOK_EOF){
		if(entry->node_count >= MAX_NODES_PER_ENTRY){
			printf("[PARSER] ERROR: too many nodes in entry '%s'\n", entry->name);
			break;
		}
		if(parser_match(p, TOK_SAY)){
			struct DialogueNode *node = &entry->node[entry->node_count++];
			memset(node, 0, sizeof(struct DialogueNode));
			if(parser_match(p, TOK_NARRATOR)){
				node->is_narrator = true;
			}

			struct Token *text = parser_consume(p);
			if(text->type == TOK_STRING){
				strncpy(node->text, text->value, MAX_TEXT_LEN - 1);
			}
			continue;
		}
		if(parser_match(p, TOK_IF)){
			struct Token *condition = parser_consume(p);
			if(parser_peek(p)->type == TOK_REPLY){
				parser_consume(p);
				struct DialogueNode *node = &entry->node[entry->node_count++];
				memset(node, 0, sizeof(struct DialogueNode));
				node->type = NODE_REPLY;
				if(condition->type == TOK_STRING){
					strncpy(node->condition, condition->value, MAX_CONDITION_LEN - 1);
				}
				struct Token *text = parser_consume(p);
				if(text->type == TOK_STRING){
					strncpy(node->text, text->value, MAX_TEXT_LEN - 1);
				}
				if(parser_match(p, TOK_GOTO)){
					struct Token *target = parser_consume(p);
					if(target->type == TOK_IDENTIFIER){
						strncpy(node->goto_entry, target->value, MAX_ENTRY_NAME - 1);
					}

				}else if(parser_match(p, TOK_EXIT)){
					node->is_exit = true;
				}
			}
			continue;
		}

		if(parser_match(p, TOK_REPLY)){
			struct DialogueNode *node = &entry->node[entry->node_count++];
			memset(node, 0, sizeof(struct DialogueNode));
			node->type = NODE_REPLY;
			struct Token *text = parser_consume(p);
			if(text->type == TOK_STRING){
				strncpy(node->text, text->value, MAX_TEXT_LEN - 1);
			}
			if(parser_match(p, TOK_GOTO)){
				struct Token *target = parser_consume(p);
				if(target->type == TOK_IDENTIFIER){
					strncpy(node->goto_entry, target->value, MAX_ENTRY_NAME - 1);
				}	
			}else if(parser_match(p, TOK_EXIT)){
				node->is_exit = true;
			}
			continue;
		}
		if(parser_match(p, TOK_SET)){
			struct DialogueNode *node = &entry->node[entry->node_count++];
			memset(node, 0, sizeof(struct DialogueNode));
			node->type = NODE_SET_FLAG;
			struct Token *flag_name = parser_consume(p);
			if(flag_name->type == TOK_IDENTIFIER){
				strncpy(node->set_flag, flag_name->value, MAX_ENTRY_NAME - 1);
			}
			struct Token *flag_val = parser_consume(p);
			node->set_flag_value = (strcmp(flag_val->value, "true") == 0 );
			continue;
		}
		p->pos++;
	}
	parser_match(p, TOK_END);
}
void parse_file(struct Parser *p, struct DialogueFile *file){
	while(parser_peek(p)->type != TOK_EOF){
		if(parser_peek(p)->type == TOK_BEGIN ||
			parser_peek(p)->type == TOK_IF){
			parse_entry(p, file);	
		}else{
			p->pos++;
		}
	}
}

bool eval_condition(const char *condition, int entity_id){
	if(!condition || condition[0] == '\0'){return true;}
	
	const char *and_pos = strstr(condition, " AND ");
	if(and_pos){
		char left_cond[MAX_CONDITION_LEN] = {0};
		char right_cond[MAX_CONDITION_LEN] = {0};
		int left_len = (int)(and_pos - condition);
		if(left_len >= MAX_CONDITION_LEN) left_len = MAX_CONDITION_LEN - 1;
		strncpy(left_cond, condition, left_len);
		strncpy(right_cond, and_pos + 5, MAX_CONDITION_LEN - 1);
		return eval_condition(left_cond, entity_id) && eval_condition(right_cond, entity_id);
	}
	const char *or_pos = strstr(condition, " OR ");
	if(or_pos){
		char left_cond[MAX_CONDITION_LEN] = {0};
		char right_cond[MAX_CONDITION_LEN] = {0};
		int left_len = (int)(or_pos - condition);
		if(left_len >= MAX_CONDITION_LEN) left_len = MAX_CONDITION_LEN - 1;
		strncpy(left_cond, condition, left_len);
		strncpy(right_cond, or_pos + 4, MAX_CONDITION_LEN - 1);
		return eval_condition(left_cond, entity_id) || eval_condition(right_cond, entity_id);
	}
	char left[64] = {0};
	char op[8]    = {0};
	char right[64] = {0};

	if(sscanf(condition, "%63s %7s %63s", left, op, right) == 3){
		int stat_index = -1;
		if      (strcmp(left, "PHYSICAL_COORDINATION") == 0) stat_index = PHYSICAL_COORDINATION;
		else if (strcmp(left, "WORD")                  == 0) stat_index = WORD;
		else if (strcmp(left, "PROB_ANALYSIS")         == 0) stat_index = PROB_ANALYSIS;
		else if (strcmp(left, "SPATIAL")               == 0) stat_index = SPATIAL;
		else if (strcmp(left, "MUSICAL")               == 0) stat_index = MUSICAL;
		else if (strcmp(left, "NATURAL")               == 0) stat_index = NATURAL;
		else if (strcmp(left, "INTERPERSONAL")         == 0) stat_index = INTERPERSONAL;
		else if (strcmp(left, "INTRAPERSONAL")         == 0) stat_index = INTRAPERSONAL;
		else if (strcmp(left, "INNOCENCE")             == 0) stat_index = INNOCENCE;
		else if (strcmp(left, "HEROISM")               == 0) stat_index = HEROISM;
		else if (strcmp(left, "LOVE")                  == 0) stat_index = LOVE;
		else if (strcmp(left, "AUTHORITARIAN")         == 0) stat_index = AUTHORITARIAN;
		else if (strcmp(left, "AC")                    == 0) stat_index = AC;
		else if (strcmp(left, "DR")                    == 0) stat_index = DR;
		else if (strcmp(left, "MAX_HP")                == 0) stat_index = MAX_HP;
		else if (strcmp(left, "INITIATIVE")            == 0) stat_index = INITIATIVE;
	
		if(stat_index != -1){
				// Fix this later to get player stat
			int val;
			int target = atoi(right);
		  	if (strcmp(op, ">=") == 0) return val >= target;
			if (strcmp(op, "<=") == 0) return val <= target;
			if (strcmp(op, ">")  == 0) return val >  target;
			if (strcmp(op, "<")  == 0) return val <  target;
			if (strcmp(op, "==") == 0) return val == target;
 			if (strcmp(op, "!=") == 0) return val != target;
		}
		if(strcmp(op, "==") == 0){
			bool flag_val = flag_get(left);
			bool target = strcmp(right, "true") == 0;
			return flag_val == target;
		}
		if(strcmp(op, "!=") == 0){
			bool flag_val = flag_get(left);
			bool target = strcmp(right, "true") == 0;
			return flag_val != target;
		}
	}
	return flag_get(condition);
}
