  #include "structs.c"
  
  void tab(int times) {

    for (int i = 0 ; i < times ; i++) {
      printf("\t");
    }
  }

  int shouldTab(char* token) {
    int length = 4;

    char* keywords[] = {
      "line",
      "then, else",
      "inner",
      "settings"
    };

    for (int i = 0 ; i < length ; i++ ) {
      if (strcmp(token,keywords[i]) == 0) 
        return 0;
    }

    return 1;
  }

  void printTree(node* tree) {

    static int tabCount = 0;
    char* token = tree->token;
    
    if ( shouldTab(token) ) {
      printf("%s\n", token);
      tab(++tabCount);
    }

    if ( tree->left ) {
      printTree( tree->left );
    }

    if ( tree->right ) {
      printTree( tree->right );
    }

    if (shouldTab(token)) {
        tabCount--;
        printf("\n");
        tab(tabCount);
    }
  }

  void printList(linkedList* list) {
    typesList* currentType;

    while (list && list->name) {

      printf("type: %s name: %s\n", list->type, list->name);
      currentType = list->types;

      while (currentType && currentType->type) {
        printf("\targ type: %s\n", currentType->type);
        currentType= currentType->next;
      }

      list = list->next;
    }
  }

  void printTreeWithLabels(node* tree){
    char* token = tree->token;

    if (shouldTab(token)){
      if(tree->funcLabel) printf("FuncLabel: %s\n", tree->funcLabel);
      if(tree->trueLabel) printf("Labels: True: %s False: %s Next: %s\n", tree->trueLabel,tree->falseLabel, tree->nextLabel);
      printf("%s\n", token);
    }

    if ( tree->left ) {
      printTreeWithLabels( tree->left );
    }

    if ( tree->right ) {
      printTreeWithLabels( tree->right );
    }
  }

  void printScope(scope* scope) {
    int count = 0;
    while (scope) {
      printf("Scope#%d\n",count++);
      if (scope->list) printList(scope->list);
      scope = scope->right;
    }
  }