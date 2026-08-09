#include <stdio.h>

#include "../include/lexer.h"
#include "../include/types.h"

ASTNode* parse_expression(Parser* parser){
  Token tok = peek_token(parser);

  if(tok.type == TOKEN_STRING){
    advance_token(parser); //consume
    
    ASTNode* new_node = malloc(sizeof(ASTNode));
    new_node->type =  NODE_STR;
    new_node->data.string_lit.value = strdup(tok.lexem);

    return new_node;
  }

  fprintf(stderr, "ERROR : Unexpected error at line %d", tok.line);
  exit(1);
}

ASTNode* parse_write_statement(Parser* parser){
    consume_token(parser, WRITE, "Expected 'Write'");
    consume_token(parser, LPAR, "Expected '(' after Write");

    ASTNode* arg = parse_expression(parser);

    consume_token(parser,RPAR, "'(' not closed");
    consume_token(parser, SEMICOLON, "';' forgotten!");
  
    ASTNode* new_node = malloc(sizeof(ASTNode));
    new_node->type = NODE_WRITE;
    new_node->data.write.argument = arg;

    return new_node;
}

ASTNode* parse_block(Parser* parser){
  consume_token(parser,BEGIN, "Expected 'Begin'");

  ASTNode *block = malloc(sizeof(ASTNode));

  block->type = NODE_BLOCK;
  block->data.block.capacity = 4;
  block->data.block.count = 0;
  block->data.block.statements = malloc(sizeof(ASTNode*) * block->data.block.capacity);

  while(peek_token(parser).type != END){
    ASTNode *stmt = NULL;

    if(peek_token(parser).type == WRITE){
      stmt = parse_write_statement(parser);
    }else{
      fprintf(stderr, "Unknown statement at line %d\n", peek_token(parser).line);
      exit(1);
    }
  
    if (block->data.block.count >= block->data.block.capacity) {
            block->data.block.capacity *= 2;
            block->data.block.statements = realloc(block->data.block.statements, sizeof(ASTNode*) * block->data.block.capacity);
    }

    block->data.block.statements[block->data.block.count++] = stmt;
  }

  consume_token(parser,END, "Expected 'END'");
  return block;
}

ASTNode* parse_program(Parser* parser) {
    consume_token(parser,ALGORITHM, "Program must start with 'Algorithm'");
    
    Token name_tok = consume_token(parser,IDENTIFIER, "Expected algorithm name");

    ASTNode *program = malloc(sizeof(ASTNode));
    program->type = NODE_PROGRAM;
    program->data.program.name = strdup(name_tok.lexem);
    program->data.program.body = parse_block(parser);

    return program;
}
