#include <stdio.h>

#include "../include/lexer.h"
#include "../include/types.h"

ASTNode *parse_expression(Parser *parser) {
  Token tok = peek_token(parser);

  if (tok.type == TOKEN_STRING) {
    advance_token(parser); // consume

    ASTNode *new_node = malloc(sizeof(ASTNode));
    if (!new_node) {
      fprintf(stderr, "Error Allocating\n");
      exit(1);
    }
    new_node->type = NODE_STR;
    new_node->data.string_lit.value = strdup(tok.lexem);

    return new_node;
  }

  fprintf(stderr, "ERROR : Unexpected error at line %d\n", tok.line);
  exit(1);
}

ASTNode *parse_write_statement(Parser *parser) {
  consume_token(parser, WRITE, "Expected 'Write'");
  consume_token(parser, LPAR, "Expected '(' after Write");

  ASTNode *arg = parse_expression(parser);

  consume_token(parser, RPAR, "')' not closed");
  consume_token(parser, SEMICOLON, "';' forgotten!");

  ASTNode *new_node = malloc(sizeof(ASTNode));
  if (!new_node) {
    fprintf(stderr, "Error Allocating\n");
    exit(1);
  }

  new_node->type = NODE_WRITE;
  new_node->data.write.argument = arg;

  return new_node;
}

DataType token_to_data(TokenType type) {
  switch (type) {
  case TOKEN_INTEGER:
    return TYPE_INTEGER;
  case TOKEN_REAL:
    return TYPE_REAL;
  case TOKEN_BOOL:
    return TYPE_BOOL;
  case TOKEN_CHAR:
    return TYPE_CHAR;

  default:
    fprintf(stderr, "Invalid token to turn into a datatype");
    exit(1);
  }
}

VarDeclNode *parse_var_declaration(Parser *parser) {
  Token name_tok = consume_token(parser, IDENTIFIER, "Expected Identifier");
  consume_token(parser, COLON, "Expected ':' after var declaration");

  Token data_tok = peek_token(parser);
  switch (data_tok.type) {
  case TOKEN_REAL:
  case TOKEN_INTEGER:
  case TOKEN_BOOL:
  case TOKEN_CHAR:
    advance_token(parser);
    break;
  default:
    fprintf(stderr, "ERROR: Unknown data type '%s'\n", data_tok.lexem);
    exit(1);
  }
  consume_token(parser, SEMICOLON, "Expected ';'");

  if (containsKey(parser->symbol_table, name_tok.lexem)) {
    fprintf(stderr, "Variable already declared\n");
    exit(1);
  }

  VarDeclNode *new_node = malloc(sizeof(VarDeclNode));
  if (!new_node) {
    fprintf(stderr, "Error Allocating\n");
    exit(1);
  }

  new_node->data_type = token_to_data(data_tok.type);
  new_node->name = strdup(name_tok.lexem);

  set(&parser->symbol_table, name_tok.lexem, data_tok.type);

  return new_node;
}

ASTNode *parse_var_block(Parser *parser) {
  consume_token(parser, VAR, "Expected 'Var'");

  ASTNode *var_node = malloc(sizeof(ASTNode));
  if (!var_node) {
    fprintf(stderr, "Error Allocating\n");
    exit(1);
  }

  var_node->type = NODE_VAR_BLOCK;
  var_node->data.var_block.capacity = 4;
  var_node->data.var_block.count = 0;
  var_node->data.var_block.declarations =
      malloc(sizeof(VarDeclNode *) * var_node->data.var_block.capacity);

  if (!var_node->data.var_block.declarations) {
    fprintf(stderr, "Error Allocating\n");
    exit(1);
  }

  while (peek_token(parser).type != BEGIN &&
         peek_token(parser).type != TOKEN_EOF) {
    VarDeclNode *decl = parse_var_declaration(parser);

    if (var_node->data.var_block.count >= var_node->data.var_block.capacity) {
      var_node->data.var_block.capacity *= 2;
      var_node->data.var_block.declarations =
          realloc(var_node->data.var_block.declarations,
                  sizeof(VarDeclNode *) * var_node->data.var_block.capacity);
      if (!var_node->data.var_block.declarations) {
        fprintf(stderr, "Error Allocating\n");
        exit(1);
      }
    }

    var_node->data.var_block.declarations[var_node->data.var_block.count++] =
        decl;
  }

  return var_node;
}

ASTNode *parse_block(Parser *parser) {
  consume_token(parser, BEGIN, "Expected 'Begin'");

  ASTNode *block = malloc(sizeof(ASTNode));

  block->type = NODE_BLOCK;
  block->data.block.capacity = 4;
  block->data.block.count = 0;
  block->data.block.statements =
      malloc(sizeof(ASTNode *) * block->data.block.capacity);

  while (peek_token(parser).type != END) {
    ASTNode *stmt = NULL;

    if (peek_token(parser).type == WRITE) {
      stmt = parse_write_statement(parser);
    } else {
      fprintf(stderr, "Unknown statement at line %d, Token : %s\n",
              peek_token(parser).line, peek_token(parser).lexem);
      exit(1);
    }

    if (block->data.block.count >= block->data.block.capacity) {
      block->data.block.capacity *= 2;
      block->data.block.statements =
          realloc(block->data.block.statements,
                  sizeof(ASTNode *) * block->data.block.capacity);
      if (!block->data.block.statements) {
        fprintf(stderr, "Error Allocating\n");
        exit(1);
      }
    }

    block->data.block.statements[block->data.block.count++] = stmt;
  }

  consume_token(parser, END, "Expected 'END'");
  return block;
}

ASTNode *parse_program(Parser *parser) {
  consume_token(parser, ALGORITHM, "Program must start with 'Algorithm'");

  Token name_tok = consume_token(parser, IDENTIFIER, "Expected algorithm name");

  ASTNode *program = malloc(sizeof(ASTNode));
  if (!program) {
    fprintf(stderr, "Error Allocating\n");
    exit(1);
  }

  program->type = NODE_PROGRAM;
  program->data.program.name = strdup(name_tok.lexem);

  if (peek_token(parser).type == VAR) {
    program->data.program.var_block = parse_var_block(parser);
  } else {
    program->data.program.var_block = NULL;
  }

  program->data.program.body = parse_block(parser);

  return program;
}
