#ifndef PARSER_H
#define PARSER_H

#include "types.h"

ASTNode* parse_expression(Parser* parser);
ASTNode* parse_write_statement(Parser* parser);
ASTNode* parse_block(Parser* parser);
ASTNode* parse_program(Parser* parser);

#endif
