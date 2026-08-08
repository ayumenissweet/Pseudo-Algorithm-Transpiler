#ifndef TYPES_H
#define TYPES_H

#include "map.h"

typedef enum {
  ALGORITHM,
  BEGIN,
  END,
  WRITE,
  LPAR,
  RPAR,
  IDENTIFIER,
  SEMICOLON,
  TOKEN_STRING,
  TOKEN_ERR,
  TOKEN_UNKNOWN,
  TOKEN_EOF,
}TokenType;

typedef struct {
  TokenType type;
  char* lexem;
  int line;
}Token;

typedef struct {
  char *file;
  int pos; //position
  int start; //used to slice
  int line; //line
  Hashmap keywords; 
}Lexer;


#endif
