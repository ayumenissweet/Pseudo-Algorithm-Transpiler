#include <stdio.h>

#include "../include/types.h"

void convert_c(ASTNode* node, FILE* F){
  switch(node->type){
    case NODE_PROGRAM:
      fprintf(F,"#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <stdbool.h>\n\n");
      fprintf(F,"int main() {\n");

      if(node->data.program.body){ //if body not null, display
        convert_c(node->data.program.body,F);
      }

      fprintf(F,"}");
      break;

    case NODE_BLOCK:
      for(size_t i = 0; i < node->data.block.count;i++){
        if(!node->data.block.statements[i]) continue;
        convert_c(node->data.block.statements[i],F);
      }
      break;

    case NODE_WRITE:
      fprintf(F,"printf(");
      if(!node->data.write.argument) break;
      convert_c(node->data.write.argument,F);
      fprintf(F,");\n");
      break;
    case NODE_STR:
      fprintf(F,"\"%s\"", node->data.string_lit.value);
      break;
  }
}
