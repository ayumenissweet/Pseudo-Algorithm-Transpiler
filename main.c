#include <stdio.h>
#include <stdlib.h>

#include "include/emitter.h"
#include "include/lexer.h"
#include "include/map.h"
#include "include/parser.h"
#include "include/types.h"

const char *enum_to_string(TokenType type) {
  switch (type) {
  case ALGORITHM:
    return "ALGORITHM";

  case BEGIN:
    return "BEGIN";

  case END:
    return "END";

  case WRITE:
    return "WRITE";

  case LPAR:
    return "LPAR";

  case RPAR:
    return "RPAR";

  case VAR:
    return "VAR";

  case IDENTIFIER:
    return "IDENTIFIER";

  case COLON:
    return "COLON";

  case SEMICOLON:
    return "SEMICOLON";

  case TOKEN_STRING:
    return "TOKEN_STRING";

  case TOKEN_INTEGER:
    return "TOKEN_INTEGER";

  case TOKEN_REAL:
    return "TOKEN_REAL";

  case TOKEN_BOOL:
    return "TOKEN_BOOL";

  case TOKEN_CHAR:
    return "TOKEN_CHAR";

  case TOKEN_ERR:
    return "TOKEN_ERR";

  case TOKEN_UNKNOWN:
    return "TOKEN_UNKNOWN";

  case TOKEN_EOF:
    return "TOKEN_EOF";

  default:
    return "INVALID_TOKEN_TYPE";
  }
}

static const char *datatype_to_string(DataType type) {
  switch (type) {
  case TYPE_INTEGER:
    return "INTEGER";

  case TYPE_REAL:
    return "REAL";

  case TYPE_BOOL:
    return "BOOL";

  case TYPE_CHAR:
    return "CHAR";

  default:
    return "UNKNOWN_TYPE";
  }
}

static void print_indent(int depth) {
  for (int i = 0; i < depth; i++) {
    printf("  ");
  }
}

static void print_var_decl(VarDeclNode *decl, int depth) {
  if (!decl) {
    print_indent(depth);
    printf("(null declaration)\n");
    return;
  }

  print_indent(depth);
  printf("VARIABLE: %s\n", decl->name);

  print_indent(depth + 1);
  printf("TYPE: %s\n", datatype_to_string(decl->data_type));
}

static void print_ast_node(ASTNode *node, int depth) {
  if (!node) {
    print_indent(depth);
    printf("(null)\n");
    return;
  }

  switch (node->type) {
  case NODE_PROGRAM:
    print_indent(depth);
    printf("PROGRAM: %s\n", node->data.program.name);

    print_indent(depth + 1);
    printf("VARIABLES:\n");

    print_ast_node(node->data.program.var_block, depth + 2);

    print_indent(depth + 1);
    printf("BODY:\n");

    print_ast_node(node->data.program.body, depth + 2);

    break;

  case NODE_BLOCK:
    print_indent(depth);

    printf("BLOCK (%zu statements)\n", node->data.block.count);

    for (size_t i = 0; i < node->data.block.count; i++) {

      print_ast_node(node->data.block.statements[i], depth + 1);
    }

    break;

  case NODE_VAR_BLOCK:
    print_indent(depth);

    printf("VAR_BLOCK (%zu declarations)\n", node->data.var_block.count);

    for (size_t i = 0; i < node->data.var_block.count; i++) {

      print_var_decl(node->data.var_block.declarations[i], depth + 1);
    }

    break;

  case NODE_WRITE:
    print_indent(depth);
    printf("WRITE\n");

    print_indent(depth + 1);
    printf("ARGUMENT:\n");

    print_ast_node(node->data.write.argument, depth + 2);

    break;

  case NODE_STR:
    print_indent(depth);

    printf("STRING: \"%s\"\n", node->data.string_lit.value);

    break;
  default:
    print_indent(depth);

    printf("UNKNOWN NODE (%d)\n", node->type);

    break;
  }
}

void print_ast(ASTNode *root) {
  printf("\n");
  printf("========================================\n");
  printf("                  AST\n");
  printf("========================================\n");

  if (!root) {
    printf("(empty)\n");
  } else {
    print_ast_node(root, 0);
  }

  printf("========================================\n");
  printf("\n");
}

int main(void) {
  char path[256] = "main.pas";

  char *source = read_file(path);

  if (!source) {
    fprintf(stderr, "ERROR: Could not read '%s'\n", path);
    return 1;
  }

  TokenArray token_arr = tokenize(source);

  Parser parser =
      (Parser){.tokens = token_arr, .current = 0, .symbol_table = initMap()};

  ASTNode *root = parse_program(&parser);

  if (!root) {
    fprintf(stderr, "ERROR: Parsing failed\n");

    free(source);
    free_token_array(&token_arr);

    return 1;
  }

  print_ast(root);

  /*
  char filename[256];

  snprintf(
    filename,
    sizeof(filename),
    "%s.c",
    root->data.program.name
  );

  FILE *F = fopen(filename, "w");

  if (!F) {
    fprintf(
      stderr,
      "ERROR: Could not create '%s'\n",
      filename
    );

    free(source);
    free_token_array(&token_arr);

    return 1;
  }

  convert_c(root, F);

  fclose(F);
  */

  free(source);
  free_token_array(&token_arr);

  return 0;
}
