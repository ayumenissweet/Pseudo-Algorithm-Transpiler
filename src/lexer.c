#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../include/types.h"
#include "../include/map.h"

char* read_file(char path[256]){
  FILE *F = fopen(path, "rb");
  if(!F) return NULL;

  if(fseek(F,0,SEEK_END) != 0) {
    fclose(F);
    return NULL;
  } //make sure the action succeeds

  int length = ftell(F);

  if(length < 0) {
    fclose(F);
    return NULL;
  }

  rewind(F); //go BACK to the beginning

  char* buffer = malloc(length + 1); //for the '\0' terminator
  if(!buffer){
    fclose(F);
    return NULL;
  }

  int bytes_read = fread(buffer,1,length, F);

  if(bytes_read != length){ //make sure we read exactly how much we were 'told'
    free(buffer);
    fclose(F);
    return NULL;
  }

  buffer[length] = '\0';
  fclose(F);
  return buffer;
}

TokenType map_token(Lexer lexer){
  int start = lexer.start;
  int end = lexer.pos;

  int len = end - start;
  
  char* slice = malloc(len + 1);
  if(!slice){
      return TOKEN_ERR;
  }

  memcpy(slice, lexer.file + start, len);
  slice[len] = '\0';

  float* value = get(lexer.keywords, slice); // hashmap containing all keywords
  free(slice);

  return (!value ? IDENTIFIER : (TokenType)(*value));
}

char peek(Lexer lexer){
  return lexer.file[lexer.pos];
}

char peek_next(Lexer lexer){
  return lexer.file[lexer.pos + 1];
}

char advance(Lexer *lexer){
  return lexer->file[lexer->pos++];
}

bool is_end(Lexer lexer){
  return (lexer.file[lexer.pos] == '\0' ? true : false);
}

bool skip_whitespace(Lexer* lexer){
  char c = peek(*lexer);
  while(c == ' ' || c == '\n' || c == '\t' || c == '\0'){
    if(c == '\n') lexer->line++;
    if(c == '\0') return false;
    (lexer->pos)++;
    c = peek(*lexer);
  }

  return true;
}

char* get_slice(char* source,int start, int end){
  int len = (end > start) ? (end - start) : 0;
  
  char* slice = malloc(len + 1);
  if(!slice){
      return NULL;
  }

  memcpy(slice, source + start, len);
  slice[len] = '\0';

  return slice;
}

Token make_token(Lexer* lexer, TokenType type, char* slice) {
  return (Token) {
    .type = type,
    .line = lexer->line,
    .lexem = slice,
  };
}

void lexer_init(Lexer* lexer, char* source){
  lexer->file = source;
  lexer->pos = 0;
  lexer->start = 0;
  lexer->line = 1;
  lexer->keywords = initMapOf({"Algorithm", ALGORITHM}, {"Begin", BEGIN}, {"End", END},{"Write", WRITE});
}

Token next_token(Lexer* lexer){
    while(peek(*lexer) != '\0'){
        if(!skip_whitespace(lexer)) break;
        lexer->start = lexer->pos;

        char c = peek(*lexer); 
        if(c == ';'){
          advance(lexer);
          return (Token){
            .type = SEMICOLON,
            .line = lexer->line,
            .lexem = ";"
          };
        }

        if(c == '('){
          advance(lexer); 
          return (Token){
            .type = LPAR,
            .line = lexer->line,
            .lexem = "("
          };
        }

        if(c == ')'){
          advance(lexer);
          return (Token){
            .type = RPAR,
            .line = lexer->line,
            .lexem = ")"
          };
        }



        if(c == '\'' || c == '"'){
          advance(lexer); //consume 
          lexer->start = lexer->pos;
            if(c == '\''){
              do {
                c = advance(lexer);
                if(c == '\0'){
                  char* slice = get_slice(lexer->file, lexer->start, lexer->pos);
                  return make_token(lexer,TOKEN_ERR, slice);
                }
              }while(c != '\'');
              char* slice = get_slice(lexer->file, lexer->start,lexer->pos - 1);
              return make_token(lexer,TOKEN_STRING,slice);
            } 

             if(c == '"'){
              do {
                c = advance(lexer);
                if(c == '\0'){
                  char* slice = get_slice(lexer->file, lexer->start, lexer->pos);
                  return make_token(lexer,TOKEN_ERR, slice);
                }
              }while(c != '"');
              char* slice = get_slice(lexer->file, lexer->start,lexer->pos - 1);
              return make_token(lexer,TOKEN_STRING,slice);
            } 
        }

        if(isalpha(c) || c == '_') { //you can start variables with _
            while(c != '\0' && !isspace(c) && c != ';' && c != '"' && c != '\'' && c != '(' && c != ')'){
              advance(lexer); //keep advancing
              c = peek(*lexer);
            }
            
            TokenType type = map_token(*lexer);
            char* slice = get_slice(lexer->file,lexer->start, lexer->pos);
            return make_token(lexer, type,slice);
        }
        while(c != '\0' && !isspace(c) && c != ';' && c != '"' && c != '\'' && c != '(' && c != ')'){
              advance(lexer); //keep advancing
              c = peek(*lexer);
            }
            char* slice = get_slice(lexer->file,lexer->start, lexer->pos);
            return make_token(lexer, TOKEN_UNKNOWN,slice);
        }
    //we reach the end and return that token
    return make_token(lexer,TOKEN_EOF,"END");
}

TokenArray tokenize(char* source){
  Lexer lexer;
  lexer_init(&lexer, source);

  TokenArray arr;
  arr.count = 0;
  arr.capacity = 64; //arbitrary starting size, grows as needed
  arr.tokens = malloc(sizeof(Token) * arr.capacity);
  if(!arr.tokens){
    arr.capacity = 0;
    return arr;
  }

  while(true){
    if(arr.count >= arr.capacity){
      int new_capacity = arr.capacity * 2;
      Token* new_tokens = realloc(arr.tokens, sizeof(Token) * new_capacity);
      if(!new_tokens) break; //bail out, caller gets whatever we collected so far
      arr.tokens = new_tokens;
      arr.capacity = new_capacity;
    }

    Token tok = next_token(&lexer);
    arr.tokens[arr.count++] = tok;

    if(tok.type == TOKEN_EOF) break;
  }

  return arr;
}

void free_token_array(TokenArray* arr){
  for(int i = 0; i < arr->count; i++){
    TokenType type = arr->tokens[i].type;
    if(type != SEMICOLON && type != LPAR && type != RPAR && type != TOKEN_EOF){
      free(arr->tokens[i].lexem);
    }
  }
  free(arr->tokens);
  arr->tokens = NULL;
  arr->count = 0;
  arr->capacity = 0;
}

Token peek_token(Parser *p) {
    return p->tokens.tokens[p->current];
}

Token advance_token(Parser *p) {
    return p->tokens.tokens[p->current++];
}

Token consume_token(Parser *p, TokenType type, const char *err_msg) {
    Token token = peek_token(p);
    if (token.type == type)
        return advance_token(p);

    fprintf(
        stderr,
        "Parse Error [Line %d]: %s\n",
        token.line,
        err_msg
    );
    exit(1);
}
