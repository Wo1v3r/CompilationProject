char* generatedCode;
int registerCount = 0;
int tabCount = 0;
int registerAccumalator = 0;

char* createRegister(){
    char num[6];
    sprintf(num, "%d", registerCount++);
    char* reg = (char*)malloc(strlen(num) + 3);
    strcpy(reg, "_t");
    strcat(reg, num);

    registerAccumalator++;
    return reg;
}

char* addLineToCode(char* line) {
  int i;
  int size = strlen(generatedCode) + strlen(line) + 1 + tabCount;
  generatedCode = (char*) realloc(generatedCode, size);

  for (i = 0 ; i < tabCount ; i++) {
    strcat(generatedCode, "\t");
  }

  strcat(generatedCode, line);
  free(line);
  return generatedCode;
}

char* createOP(char* op, char* currentReg, char* leftReg, char* rightReg){
  int len = strlen(currentReg) + strlen(leftReg) + strlen(op) + strlen(rightReg) + 8;
  char* opLine = (char*)malloc(len);

  strcpy(opLine,currentReg);
  strcat(opLine, " = ");
  strcat(opLine,leftReg);
  strcat(opLine, " ");
  strcat(opLine,op);
  strcat(opLine, " ");
  strcat(opLine,rightReg);
  strcat(opLine, "\n");

  return opLine;
}

char* createAssign(char* currentReg, char* rightReg) {
  int len = strlen(currentReg) + strlen(rightReg) + 5;
  char* line = (char*)malloc(len);

  strcpy(line, currentReg);
  strcat(line, " = ");
  strcat(line, rightReg);
  strcat(line, "\n");

  return line;
}

char* unarOP(char* token){
  if (strcmp(token, "reference") == 0) return "&";
  if (strcmp(token, "pointer") == 0) return "^";
  
  return token;
}

char* createUNAR(char* op, char* currentReg, char* leftReg) {
  int len;
  op = unarOP(op);
  len = strlen(currentReg) + strlen(leftReg) + strlen(op) + 5;
  char* line = (char*)malloc(len);

  strcpy(line, currentReg);
  strcat(line," = ");
  strcat(line, op);
  strcat(line, " ");
  strcat(line, leftReg);
  strcat(line, "\n");

  return line;
}

char* createMem(node* tree, char** currentReg, char* leftReg, char* rightReg) {
  char* offsetLine = createOP("+", *currentReg, leftReg, rightReg);
  addLineToCode(offsetLine);

  char* newReg = createRegister();
  char* line = createUNAR("pointer", newReg, *currentReg);

  *currentReg = newReg;

  return line;
}

char* createReturn(node* tree, char* leftReg) {
  int len = strlen("return ") + 1;
  char* reg = "";

  if (leftReg) {
    len += strlen(leftReg);
    reg = leftReg;
  }

  char *line = (char*) malloc(len);

  strcpy(line, "return ");
  strcat(line, reg);
  strcat(line, "\n");

  return line;
}

char* createVarLine(char* ident, char* currentReg){
  int len = strlen(ident) + strlen(currentReg) + 5;
  char *line = (char*) malloc(len);

  strcpy(line, currentReg);
  strcat(line, " = ");
  strcat(line, ident);
  strcat(line, "\n");

  return line;
}

char* createIfLine(char* leftReg, node* tree) {
  char* falseLabel = tree->left->falseLabel;
  char* trueLabel = tree->right->left->trueLabel;

  if (!falseLabel)
    falseLabel = tree->left->nextLabel;

  int len = strlen("ifZ ") + strlen(falseLabel) +strlen(trueLabel) + strlen(" Goto   ") + strlen(leftReg) + 3;
  char *line = (char*) malloc(len);

  strcpy(line, "ifZ ");
  strcat(line, leftReg);  
  strcat(line, " Goto ");
  strcat(line, falseLabel);
  strcat(line, ":\n");
  strcat(line, trueLabel);
  strcat(line, ": ");  
  return line;
}

char* createFunction(node* tree) {
  char* funcLabel = tree->right->right->funcLabel;
  int len = strlen(funcLabel) + 3;
  char *line = (char*) malloc(len);  

  strcpy(line, funcLabel);
  strcat(line, ":\n");

  return line;
}

char* beginFunction(int size){
  char* begin = "BeginFunc ";
  char num[5];
  sprintf(num, "%d", size);
  char *line = (char*) malloc(strlen(begin) + strlen(num) + 3);

  strcpy(line, begin);
  strcat(line, num);
  strcat(line, "\n");
  return line;
}


char* insertBeginFunc(int index) {
    
  char* rest = generatedCode + index + tabCount + 1;
  char* temp = (char*) malloc(strlen(rest) + 1);

  strcpy(temp,rest);

  generatedCode[index + 1] = '\0';

  addLineToCode(beginFunction(registerAccumalator));
  addLineToCode(temp);

  registerAccumalator = 0;
  return generatedCode;
}

char* endFunction(){
  char* end = "EndFunc";
  char *line = (char*) malloc(strlen(end) + 2);

  strcpy(line, end);
  strcat(line, "\n");  
  return line;
}

char* beginWhile(node* tree, char* leftReg) {
  char* trueLabel = tree->trueLabel;
  char* falseLabel = tree->falseLabel;
  
  int len =  
      strlen(trueLabel) 
    + strlen(": ifZ ") 
    + strlen(leftReg)
    + strlen(" Goto ")
    + strlen(falseLabel);

  char *line = (char*) malloc(len + 2);

  strcpy(line, trueLabel);
  strcat(line, ": ifZ ");
  strcat(line, leftReg);
  strcat(line, " Goto ");
  strcat(line, falseLabel);
  strcat(line, "\n");
  return line;
}

char* endWhile(node* tree) {

  char* trueLabel = tree->trueLabel;
  char* falseLabel = tree->falseLabel;
  
  int len = 5 + strlen("Goto ") + strlen(trueLabel) + strlen(falseLabel);
  char *line = (char*) malloc(len);

  strcpy(line, "Goto ");
  strcat(line, trueLabel);
  strcat(line, "\n");
  strcat(line, falseLabel);  
  strcat(line, ": ");

  return line;
}


char* beginDoWhile(node* tree, char* leftReg) {
  char* trueLabel = tree->trueLabel;
  
  int len =  
      strlen(trueLabel) 
    + strlen(": ");

  char *line = (char*) malloc(len + 1);

  strcpy(line, trueLabel);
  strcat(line, ": ");
  return line;
}

char* endDoWhile(node* tree, char* leftReg) {

  char* trueLabel = tree->trueLabel;
  
  int len = 1 + strlen("If  Goto \n") + strlen(trueLabel) + strlen(leftReg);
  char *line = (char*) malloc(len);

  strcpy(line, "If ");
  strcat(line, leftReg);
  strcat(line, " Goto ");
  strcat(line, trueLabel);  
  strcat(line, "\n");

  return line;
}

char* addGonetoLabel(node* tree)  {
    char* someLabel = NULL;
    char* line = NULL;

    if ( tree->right->right )
      someLabel = tree->nextLabel;
    else
      someLabel = tree->falseLabel;

    line = (char*) malloc(strlen(someLabel) + 3);
    strcpy(line, "");
    strcat(line, someLabel);
    strcat(line, ": ");

    return line;
}


char* addThenElseGoto(node* tree){
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

    char* tmp = (char*) malloc(len + 7);
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

    return tmp;
}



char* createForCondLine(char* leftReg, node* tree) {
  char* falseLabel = tree->left->falseLabel;
  char* trueLabel = tree->left->trueLabel;

  if (!falseLabel)
    falseLabel = tree->left->nextLabel;

  int len = strlen("ifZ ") + strlen(falseLabel) +strlen(trueLabel) + strlen(" Goto   ") + strlen(leftReg) + 3;
  char *line = (char*) malloc(len);

  strcpy(line, "ifZ ");
  strcat(line, leftReg);  
  strcat(line, " Goto ");
  strcat(line, falseLabel);
  strcat(line, ":\n");
  strcat(line, trueLabel);
  strcat(line, ": ");  
  return line;
}

char* createForCond(node* tree, char* condReg) {
  char* trueLabel = tree->trueLabel;
  int len = 1 + strlen(": ") + strlen(trueLabel);
  char *line = (char*) malloc(len);

  strcpy(line, trueLabel);
  strcat(line, ": ");

  addLineToCode(line);

  line = createForCondLine(condReg, tree->left->right);
  return line;
}

char* createForGoto(node* tree){

  char* trueLabel = tree->trueLabel;
  char* falseLabel = tree->left->right->left->falseLabel;

  if (!falseLabel) 
    falseLabel = tree->left->right->left->nextLabel;

  int len = 1 + strlen("Goto :   \n") + strlen(trueLabel) + strlen(falseLabel);
  char *line = (char*) malloc(len);

  strcpy(line, "Goto ");
  strcat(line, trueLabel);
  strcat(line," \n");
  strcat(line,falseLabel);  
  strcat(line,": ");  

  return line;
}


int funcCallByteSize(node* tree){
  int byteSizeCount = 0;

  while(tree){
    //FIXME: Should depend on type etc..
    byteSizeCount++;
    tree = tree->right;
  }

  return byteSizeCount;
}

char* pushParams(node* tree){
  int len = 1;
  char *line = (char*) malloc(len);
  char* push = "PushParam ";

  while(tree){
      char* reg = generateTree(tree->left);

      len = strlen(line) + strlen(push) + strlen(reg) + 2;
      line = (char*) realloc(line,len);

      strcat(line,push);
      strcat(line,reg);
      strcat(line, "\n");

    tree = tree->right;
  }

  return line;
};

char* createFunctionCall(node* tree, char* currentReg){

  char* funcName = tree->left->token;
  int len = 4 + strlen(currentReg) + strlen(" = LCall _\n") + strlen("\nPopParams ") + strlen(funcName);
  int callSize = funcCallByteSize(tree->right);
  char num[6];
  sprintf(num, "%d", callSize);

  char *line = (char*) malloc(len);
  char* params = pushParams(tree->right);

  addLineToCode(params);

  strcpy(line, currentReg);
  strcat(line," = LCall _");
  strcat(line,funcName);
  strcat(line,"\nPopParams ");
  strcat(line,num);
  strcat(line,"\n");
  

  return line;
}

char* createAnd(node* tree) {
  int len = 0;
  char *line = NULL;

  char* leftReg = generateTree(tree->left);
  len = strlen(leftReg) +  strlen(tree->falseLabel) + strlen("Ifz    Goto \n") + 1;
  line = (char*) malloc(len);
  strcpy(line,"Ifz ");
  strcat(line,leftReg);
  strcat(line," Goto ");
  strcat(line,tree->falseLabel);
  strcat(line,"\n");
  addLineToCode(line);
  
  char* rightReg = generateTree(tree->right);

  return rightReg;
}


char* createOr(node* tree) {
  int len = 0;
  char *line = NULL;
  char* leftReg = generateTree(tree->left);

  len = strlen(leftReg) +  strlen(tree->trueLabel) + strlen("If    Goto \n") + 1;
  line = (char*) malloc(len);

  strcpy(line,"If ");
  strcat(line,leftReg);
  strcat(line," Goto ");
  strcat(line,tree->trueLabel);
  strcat(line,"\n");

  addLineToCode(line);
  
  if(isAnd(tree->left->token)) {
    int len2 = strlen(tree->left->falseLabel) + strlen(": ") + 1;
    char* line2 = (char*) malloc(len2);

    strcpy(line2, tree->left->falseLabel);
    strcat(line2, ": ");
    addLineToCode(line2);
  }

  char* rightReg = generateTree(tree->right);

  return rightReg;
}