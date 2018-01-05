#include "labelizer.c"


char* generatedCode;

int isOP(char* token) {
    int length = 4;

    char* keywords[] = {
      "+", "-", "*", "/"
    };

    for (int i = 0 ; i < length ; i++ ) {
      if (strcmp(token,keywords[i]) == 0) 
        return 1;
    }

    return 0;
}

void generateCode(node* tree){
  generatedCode = (char*) malloc(10);
  generateTree(tree);
}
void generateTree(node* tree) {
    char *token = tree->token;

    if ( tree->left ) {
      generateTree( tree->left);
    }

    if ( tree->right ) {
      generateTree( tree->right);
    }
}