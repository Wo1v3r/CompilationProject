#include "labelizer.c"


char* generatedCode;
int registerCount = 0;
int tabCount = 0;


char* createRegister(){
    char num[6];
    sprintf(num, "%d", registerCount++);
    char* reg = (char*)malloc(strlen(num) + 3);
    strcpy(reg, "_t");
    strcat(reg, num);

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

char* endFunction(){
  char* end = "EndFunc";
  char *line = (char*) malloc(strlen(end) + 2);

  strcpy(line, end);
  strcat(line, "\n");  
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


char* insertBeginFunc(int index, int size) {
    
  char* rest = generatedCode + index + tabCount + 1;
  char* temp = (char*) malloc(strlen(rest) + 1);

  strcpy(temp,rest);

  generatedCode[index + 1] = '\0';

  addLineToCode(beginFunction(size));
  addLineToCode(temp);

  return generatedCode;
}


int isNumber(char* token) {
    if (strcmp(token,"0") == 0) return 1;

    if (atoi(token)) return 1;

    return 0;
}


int isFunctionDef(char* token) {
  return (strcmp(token,"function def") == 0);
}

int isMem(char* token) {
  return (strcmp(token,"[]") == 0);
}

int isControl(char* token) {
  return (strcmp(token,"if") == 0);
}

int isReturn(char* token) {
  return (strcmp(token,"return") == 0);
}

char* unarOP(char* token){
  if (strcmp(token, "reference") == 0) return "&";
  if (strcmp(token, "pointer") == 0) return "^";
  
  return token;
}

int isAssignment(char* token) {
  return strcmp(token, "=") == 0;
}

int isUNAR(char* token) {
    int length = 4;

    char* keywords[] = {
      "pointer", "reference", "abs", "!"
    };

    for (int i = 0 ; i < length ; i++ ) {
      if (strcmp(token,keywords[i]) == 0) 
        return 1;
    }

    return 0;
}


int isOP(char* token) {
    int length = 10;

    char* keywords[] = {
      "+", "-", "*", "/", "<", "<=" , ">", ">=", "==", "!="
    };

    for (int i = 0 ; i < length ; i++ ) {
      if (strcmp(token,keywords[i]) == 0) 
        return 1;
    }

    return 0;
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

char* createFunction(node* tree) {
  char* funcLabel = tree->right->right->funcLabel;
  int len = strlen(funcLabel) + 3;
  char *line = (char*) malloc(len);  

  strcpy(line, funcLabel);
  strcat(line, ":\n");

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

int shouldSkip(char* token) {

    int length = 1;

    char* keywords[] = {
        "decl list "
    };

    for (int i = 0 ; i < length ; i++ ) {
      if (strcmp(token,keywords[i]) == 0) 
        return 1;
    }

    return 0;
}



char* generateTree(node* tree) {
    static int registerAccumalator = 0;
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
      insertBeginFunc(beginIndex, registerAccumalator);
      registerAccumalator = 0;
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
      registerAccumalator++;      
      currentReg = createRegister();
      line = createOP(token,currentReg, leftReg, rightReg);
      addLineToCode(line);
    }

    else if (isUNAR(token)) {
      registerAccumalator++;
      currentReg = createRegister();
      line = createUNAR(token,currentReg,leftReg);
      addLineToCode(line);
    }

    else if (isNotKeyword(token) && isIdent(token)) {
      registerAccumalator++;
      currentReg = createRegister();
      line = createVarLine(token, currentReg);
      addLineToCode(line);

      return currentReg;
    }

    else if (isMem(token)) {
      registerAccumalator += 2;      
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

//////////
void generateCode(node* tree){
  generatedCode = (char*) malloc(10);
  generatedCode[0] = '\0';
  generateTree(tree);

  printf("%s\n", generatedCode);
  free(generatedCode);
}