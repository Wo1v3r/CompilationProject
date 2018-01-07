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