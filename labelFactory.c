
char* createLabel();
char* appendToFunction(char* funcName);


int labelCount = 0;

char* createLabel(){
    char num[6];
    sprintf(num, "%d", labelCount++);
    char* label = (char*)malloc(2+strlen(num));
    strcpy(label, "L");
    strcat(label, num);

    return label;
}

char* appendToFunction(char* funcName){
  char* newLabel = (char*)malloc(strlen(funcName) + 2 );

  strcpy(newLabel,"_");
  strcat(newLabel,funcName);

  return newLabel;
}