#include <stdio.h>
#include <stdlib.h>

#include "include/types.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/emitter.h"

int main(void)
{
    char path[256] = "main.pas";

    char *source = read_file(path);

    if (!source) {
        return 1;
    }

    TokenArray token_arr = tokenize(source);

    Parser parser = (Parser) {
        .tokens = token_arr,
        .current = 0
    };

    ASTNode *root = parse_program(&parser);

    if (!root) {
        printf("ERROR : Parsing Failed\n");
        free(source);
        free_token_array(&token_arr);
        return 1;
    }

    char filename[256];

    snprintf(
        filename,
        sizeof(filename),
        "%s.c",
        root->data.program.name
    );

    FILE *F = fopen(filename, "w");
      if (!F) {
          return 1;
     }
     
    convert_c(root, F);
    fclose(F);
     

    free(source);
    free_token_array(&token_arr);

    return 0;
}
