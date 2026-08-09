#ifndef LEXER_H
#define LEXER_H

#include "types.h"

char* read_file(char path[256]);
void lexer_init(Lexer* lexer, char* source);
Token next_token(Lexer* lexer);
TokenArray tokenize(char* source);
void free_token_array(TokenArray* arr);
Token peek_token(Parser *p);
Token advance_token(Parser *p);
Token consume_token(Parser *p, TokenType type, const char *err_msg);

#endif
