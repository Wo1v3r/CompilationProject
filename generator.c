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

    if (isFunctionCall(token)) {
      line = createFunctionCall(tree);
      addLineToCode(line);
      return NULL;
    }

    if (isNumber(token)) { 
      return token;
    }

    if (isFor(token)) {
      char* initReg = generateTree(tree->left->left->left);
      char* condReg = generateTree(tree->left->right->left->left);

      line = createForCond(tree, condReg);
      addLineToCode(line);

      generateTree(tree->right);
      char* updateReg = generateTree(tree->left->right->right);

      line = createForGoto(tree);
      addLineToCode(line);
      return NULL;
    }

    if ( tree->left ) {
      leftReg = generateTree( tree->left);
    }
    
    if (isThenElse(token)) {
      line = addThenElseGoto(tree);
      addLineToCode(line);
    }

    if (isIf(token)) {
      line = createIfLine(leftReg, tree);
      addLineToCode(line);
    }

    if (isWhile(token)) {
      line = beginWhile(tree, leftReg);
      addLineToCode(line);
    }

    if (isDoWhile(token )) {
      line = beginDoWhile(tree, leftReg);
      addLineToCode(line);
    }

    if ( tree->right ) {
      rightReg = generateTree( tree->right);
    }

    if (isWhile(token)) {
      line = endWhile(tree);
      addLineToCode(line);
    }

    if (isDoWhile(token )) {
      line = endDoWhile(tree, leftReg);
      addLineToCode(line);
    }

    if (isIf(token)) {
      line = addGonetoLabel(tree);
      addLineToCode(line);
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

    return currentReg;
}

void generateCode(node* tree){
  generatedCode = (char*) malloc(10);
  generatedCode[0] = '\0';
  generateTree(tree);

  printf("%s\n", generatedCode);
  free(generatedCode);
}