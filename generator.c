void createLabel(node* node){
    printf("TOKEN: %s\n",node->token);
    char num[400];
    sprintf(num, "%d", linenum);
    char* label = (char*)malloc(2+strlen(num));
    strcpy(label, "L");
    strcat(label, num);
    node -> label = label;
}