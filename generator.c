#include "labelizer.c"


char* generatedCode;
int registerCount = 0;


char* createRegister(){
    char num[6];
    sprintf(num, "%d", registerCount++);
    char* reg = (char*)malloc(strlen(num) + 3);
    strcpy(reg, "_t");
    strcat(reg, num);

    return reg;
}

char* addLineToCode(char* line) {
  int size = strlen(generatedCode) + strlen(line) + 1;
  generatedCode = (char*) realloc(generatedCode, size);

  strcat(generatedCode, line);
  free(line);
  return generatedCode;
}

int isNumber(char* token) {
    if (strcmp(token,"0") == 0) return 1;

    if (atoi(token)) return 1;

    return 0;
}


int isMem(char* token) {
  return (strcmp(token,"[]") == 0);
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

char* createVarLine(char* ident, char* currentReg){
  int len = strlen(ident) + strlen(currentReg) + 5;
  char *line = (char*) malloc(len);

  strcpy(line, currentReg);
  strcat(line, " = ");
  strcat(line, ident);
  strcat(line, "\n");

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
    char *token = tree->token;
    char *line = NULL;
    char *currentReg = NULL, *leftReg = NULL, *rightReg = NULL;


    if(shouldSkip(token)) {
      //TODO://
      return NULL;
    }

    if (isNumber(token)) { 
      return token;
    }
    if ( tree->left ) {
      leftReg = generateTree( tree->left);
    }

    if ( tree->right ) {
      rightReg = generateTree( tree->right);
    }
    
    if (isAssignment(token)) {
      currentReg = leftReg;
      line = createAssign(currentReg, rightReg);
      addLineToCode(line);
    }
    if (isOP(token)) {
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
      //TODO: SHOULD NOT SHOW IF NOT IN ASSIGNMENT
      currentReg = createRegister();
      line = createMem(tree, &currentReg, leftReg, rightReg);
      addLineToCode(line);
    }
    return currentReg;
}


//////////

void generateCode(node* tree){
  generatedCode = (char*) malloc(10);
  generateTree(tree);

  printf("%s\n", generatedCode);
  free(generatedCode);
}