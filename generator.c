#include "labelizer.c"
#include "codeFactory.c"
#include "nodeIdentifier.c"

char* generateTree(node* tree) {
    char *token = tree->token;
    char *line = NULL;
    char *currentReg = NULL, *leftReg = NULL, *rightReg = NULL;
    int beginIndex = 0;

    if(shouldSkip(token)) {
      //TODO://
      return NULL;
    }

    else if (isFunctionDef(token)) {
      line = createFunction(tree);
      addLineToCode(line);
      beginIndex = strlen(generatedCode) - 1;
      tabCount++;
      tree = tree->right->right;
    };

    if (isNumber(token)) { 
      return token;
    }

    if ( tree->left ) {
      leftReg = generateTree( tree->left);
    }
    
    if (strcmp(token, "then, else") == 0) {

      int len = 0;
      char* trueLabel = NULL;
      char* nextLabel = NULL;

      if (tree->right) {
        trueLabel = tree->right->trueLabel;
        len += strlen(trueLabel);
      }

      if (tree->right) {
        nextLabel = tree->right->nextLabel;
        len += strlen(nextLabel);
      }

      char* tmp = (char*) malloc(len+14);
      strcpy(tmp, "");

      if (nextLabel) {
        strcat(tmp, "Goto ");
        strcat(tmp, nextLabel);
        strcat(tmp, "\n");
      }

      if (trueLabel) {
        strcat(tmp, trueLabel);
        strcat(tmp, ": ");
      }

      addLineToCode(tmp);
    }

    if (isControl(token)) {
      line = createIfLine(leftReg, tree);
      addLineToCode(line);
    }

    if ( tree->right ) {
      rightReg = generateTree( tree->right);
    }

    if (isControl(token)) {
      char* someLabel = NULL;
      char* tmp = NULL;

      if ( tree->right->right )
        someLabel = tree->nextLabel;
      else
        someLabel = tree->falseLabel;

      tmp =  (char*) malloc(strlen(someLabel) + 3);
      strcpy(tmp, "");
      strcat(tmp, someLabel);
      strcat(tmp, ": ");

      addLineToCode(tmp);
    }

    if (isFunctionDef(token)) {
      insertBeginFunc(beginIndex);
      line = endFunction();
      addLineToCode(line);
      tabCount--;
    }

    else if (isAssignment(token)) {
      currentReg = leftReg;
      line = createAssign(currentReg, rightReg);
      addLineToCode(line);
    }

    else if (isOP(token)) {
      currentReg = createRegister();
      line = createOP(token,currentReg, leftReg, rightReg);
      addLineToCode(line);
    }

    else if (isUNAR(token)) {
      currentReg = createRegister();
      line = createUNAR(token,currentReg,leftReg);
      addLineToCode(line);
    }

    else if (isNotKeyword(token) && isIdent(token)) {
      currentReg = createRegister();
      line = createVarLine(token, currentReg);
      addLineToCode(line);

      return currentReg;
    }

    else if (isMem(token)) {   
      currentReg = createRegister();
      line = createMem(tree, &currentReg, leftReg, rightReg);
      addLineToCode(line);
    }

    else if (isReturn(token)){
      line = createReturn(tree, leftReg);
      addLineToCode(line);
    }

    // if (line) addLineToCode(line);
    return currentReg;
}

void generateCode(node* tree){
  generatedCode = (char*) malloc(10);
  generatedCode[0] = '\0';
  generateTree(tree);

  printf("%s\n", generatedCode);
  free(generatedCode);
}