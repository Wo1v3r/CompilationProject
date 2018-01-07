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
  if (!falseLabel) 
    falseLabel = tree->left->nextLabel;

  int len = strlen("ifZ ") + strlen(falseLabel) + strlen(" Goto ") + strlen(leftReg);
  char *line = (char*) malloc(len);

  strcpy(line, "ifZ ");
  strcat(line, leftReg);  
  strcat(line, " Goto ");
  strcat(line, falseLabel);
  strcat(line, ":\n");

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