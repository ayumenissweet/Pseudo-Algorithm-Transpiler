#ifndef LEXER_H
#define LEXER_H

#include "types.h"

char* read_file(char path[256]);
void lexer_init(Lexer* lexer, char* source);
Token next_token(Lexer* lexer);

#endif
