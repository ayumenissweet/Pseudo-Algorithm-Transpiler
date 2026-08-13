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
  VAR,
  IDENTIFIER,
  COLON,
  SEMICOLON,
  TOKEN_STRING,
  TOKEN_INTEGER,
  TOKEN_REAL,
  TOKEN_BOOL,
  TOKEN_CHAR,
  TOKEN_ERR,
  TOKEN_UNKNOWN,
  TOKEN_EOF,
} TokenType;

typedef enum {
  TYPE_INTEGER,
  TYPE_REAL,
  TYPE_BOOL,
  TYPE_CHAR,
} DataType;

typedef struct {
  TokenType type;
  char *lexem;
  int line;
} Token;

typedef struct {
  char *name;
  DataType data_type;
} VarDeclNode;

typedef struct {
  char *file;
  int pos;   // position
  int start; // used to slice
  int line;  // line
  Hashmap keywords;
} Lexer;

typedef struct {
  Token *tokens;
  int count;
  int capacity;
} TokenArray;

typedef struct {
  TokenArray tokens;
  int current;
  Hashmap symbol_table;
} Parser;

typedef enum {
  NODE_PROGRAM,
  NODE_BLOCK,
  NODE_VAR_BLOCK,
  NODE_WRITE,
  NODE_STR,
} NodeType;

typedef struct ASTNode ASTNode;

typedef struct {
  char *name;
  ASTNode *var_block;
  ASTNode *body;
} ProgramNode;

typedef struct {
  VarDeclNode **declarations;
  size_t count;
  size_t capacity;
} VarBlockNode;

typedef struct {
  ASTNode **statements;
  size_t count;
  size_t capacity;
} BlockNode;

typedef struct {
  ASTNode *argument;
} WriteNode;

typedef struct {
  char *value;
} StringLiteralNode;

struct ASTNode {
  NodeType type;
  union {
    ProgramNode program;
    BlockNode block;
    WriteNode write;
    StringLiteralNode string_lit;
    VarBlockNode var_block;
  } data;
};

#endif
