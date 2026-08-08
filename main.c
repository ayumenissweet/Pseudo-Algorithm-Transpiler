#include <stdio.h>

#include "include/lexer.h"
#include "include/types.h"
#include "include/map.h"

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case ALGORITHM:    return "ALGORITHM";
        case BEGIN:        return "BEGIN";
        case END:          return "END";
        case WRITE:        return "WRITE";
        case LPAR:         return "LPAR";
        case RPAR:         return "RPAR";
        case IDENTIFIER:   return "IDENTIFIER";
        case SEMICOLON:    return "SEMICOLON";
        case TOKEN_STRING: return "TOKEN_STRING";
        case TOKEN_ERR:    return "TOKEN_ERR";
        case TOKEN_UNKNOWN: return "TOKEN_UNKNOWN";
        case TOKEN_EOF:    return "TOKEN_EOF";
        default:           return "UNKNOWN_TOKEN";
    }
}

int main(){
    char filepath[256] = "main.pas";
    Token next;

    char* buffer = read_file(filepath);
    Lexer lexer; 
    lexer_init(&lexer, buffer);

    next =next_token(&lexer);
    while(next.type != TOKEN_EOF){
      printf("Type : %s\nLexem : %s\nLine: %d\n",token_type_to_string(next.type) , next.lexem, next.line);
      next = next_token(&lexer);
    }
}
