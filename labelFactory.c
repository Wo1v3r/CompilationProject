
char* createLabel();
char* appendToFunction(char* label, char* funcName);


int labelCount = 0;

char* createLabel(){
    char num[6];
    sprintf(num, "%d", labelCount++);
    char* label = (char*)malloc(2+strlen(num));
    strcpy(label, "L");
    strcat(label, num);

    return label;
}

char* appendToFunction(char* label, char* funcName){
  char* newLabel = (char*)malloc(strlen(label)+strlen(funcName) + 2 );

  strcpy(newLabel,funcName);
  strcat(newLabel,"_");
  strcat(newLabel,label);

  return newLabel;
}