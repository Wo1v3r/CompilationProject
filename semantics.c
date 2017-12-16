  #include "print.c"
  
  int isNotKeyword(char* token){
    int length = 36;

    char* keywords[] = {
      "boolean", "charp"  , "char"   , "void"   , "intp" , "int" , "string" ,"if","false","true",
      "else"   ,"while"  ,"do","for","return","null","then, else",
      "line","if","do while","block","decl list ","function call","settings","dec_list",
      "ident_list","expr_list","setup","inner","init","conditions","update","reference","pointer","declaration","array","[]"
    };

    for (int i = 0 ; i < length ; i++ ) {
      if (strcmp(token,keywords[i]) == 0) 
        return 0;
    }

    return 1;
  }

  int isIdent(char* token){
    regex_t regex;
    int reti = regcomp(&regex, "[a-zA-Z_][0-9a-zA-Z_]*", 0);

    if (reti) {
      fprintf(stderr, "Could not compile regex\n");
      exit(1);
    }

    reti = regexec(&regex, token, 0, NULL, 0);

    if (!reti) {
      return 1;
    }
    else if (reti == REG_NOMATCH) {
      return 0;
    }
    else {
      printf("Regex match failed\n");
      exit(1);
    }
  }

  char* searchList (char* token, linkedList* list) {
    if (!list) return NULL;

    if (list->name && strcmp(token,list->name) == 0) {
        return list->type;
    };

    return searchList(token, list->next);
  }

  char* typeOfInScope(char* token, scope* scope) {
    if (!scope) return NULL;
    return searchList(token,scope->list);
  }

  char* typeOf(char* token, scope* scope) {
    char* type;

    if (!scope) return NULL;

    type = typeOfInScope(token,scope);
    if (type) return type;

    return typeOf(token,scope->left);
  }


  typesList* searchListForTypes (char* token, linkedList* list) {
    if (!list) return NULL;

    if (list->name && strcmp(token,list->name) == 0) {
        return list->types;
    };

    return searchListForTypes(token, list->next);
  }

  typesList* typesOfInScope(char* token, scope* scope) {
    if (!scope) return NULL;
    return searchListForTypes(token,scope->list);
  }

  typesList* typesOf(char* token, scope* scope) {
    typesList* types;

    if (!scope) return NULL;

    types = typesOfInScope(token,scope);
    if (types) return types;

    return typesOf(token,scope->left);
  }

  void declaration(char* type, char* name, typesList* types, scope* currentScope){
    linkedList* list = currentScope->current;

    if (typeOfInScope(name,currentScope)) {
      printf("ERROR: %s was already defined in this scope!\n", name);
      return;
    }

    list->next = makeLink(NULL,NULL,NULL);
    list->name = name;
    list->type = type;
    list->types = types;
    currentScope->current = list->next;

  }

  void decl_list(node* tree, scope* currentScope) {
    char *type, *name;
    if (!tree || !tree ->left) return;

    type = tree->left->left->token;
    name = tree->left->right->token;

    declaration(type,name,NULL,currentScope);
    decl_list(tree->right, currentScope);
  }

  void declarationList(char* type, node* identList, scope* currentScope) {
      char* name;
      if (!identList) return;

      name = identList->left->token;
      identList = identList->right;

      declaration(type,name,NULL,currentScope);

      return declarationList(type,identList, currentScope);
  }

  int isVoid(node* tree ,typesList* types) {

    if (!tree || !tree->left) {
      types->type = (char*) malloc(5);
      strcpy(types->type,"void");
      return 1;
    }
    return 0;
  }

  void getTypes(node* tree, typesList* types) {
    char *type;

    if (!tree) return;

    type = tree->left->left->token;

    types-> type = type;
    types -> next = (typesList*)malloc(sizeof(typesList));

    return getTypes(tree->right, types -> next);
  }

  int isBoolean(char* token) {
    int length = 11;

    char* keywords[] = {
      "||", "&&","==","!=","!","<","<=",">",">=","true","false"
    };

    for (int i = 0 ; i < length ; i++ ) {
      if (strcmp(token,keywords[i]) == 0) 
        return 1;
    }

    return 0;
  }

  int isInt(char* token) {
    int length = 5;

    char* keywords[] = {
      "+", "-", "*", "/", "abs"
    };

    if (strcmp(token,"0") == 0) return 1;

    if (atoi(token)) return 1;

    for (int i = 0 ; i < length ; i++ ) {
      if (strcmp(token,keywords[i]) == 0) 
        return 1;
    }

    return 0;
  }

  int equality(char* token) {
    int length = 2 ;

    char* keywords[] = {
      "!=", "=="
    };

    for (int i = 0 ; i < length ; i++) {
      if (strcmp(token,keywords[i]) == 0) return 1;
    }

    return 0;
  }

  int boolBool(char* token) {
    int length = 2;

    char* keywords[] = {
      "||", "&&"
    };

    for (int i = 0 ; i < length ; i++ ) {
      if (strcmp(token,keywords[i]) == 0) 
        return 1;
    }

    return 0;
  }

  int intInt(char* token) {
      int length = 6;

      char* keywords[] = {
        ">", "<", ">=", "<=", "*", "/"
      };

      for (int i = 0 ; i < length ; i++ ) {
        if (strcmp(token,keywords[i]) == 0) 
          return 1;
      }

      return 0;
  }

  char* sameTypes(char* token){
    if (intInt(token)) return "int";
    if (boolBool(token)) return "boolean";
    if (equality(token)) return "equality";

    return NULL;
  }

  char* unarType(char* token) {
    if (strcmp (token, "!") == 0) return "boolean ";
    if (strcmp (token, "abs") == 0) return "int | string ";
    if (strcmp (token, "pointer") == 0) return "intp | charp ";
    if (strcmp (token, "reference") == 0) return "int | char "; // | string[]
    // if (strcmp (token, "[]"))

    return NULL;
  }

  char* differentTypes(char* token) {
    if (strcmp(token, "+") == 0) return "int | intp | charp ";
    if (strcmp(token, "-") == 0) return "int | intp | charp ";

    return NULL;
  }

  void semantizeDeclList(node * tree, scope* currentScope){
      char* type = tree->left->token;
      declarationList(type, tree->right, currentScope);
  }
  
  void semantizeDeclaration(node* tree, scope* currentScope){
      char *type, *name;

      type = tree->left->token;
      name = tree->right->token;

      declaration(type,name,NULL,currentScope);
  }

  void semantizeIdentifier(node* tree, scope* currentScope) {
      char* type = typeOf(tree->token,currentScope);

      if (type){
        printf("TypeOf %s: %s\n",tree->token,type);
      }
      else {
        printf("Error: %s was used but not defined!\n", tree->token);
      }
  }

  void semantizeFunctionDef(node* tree, scope* currentScope){
      char *name, *type;
      typesList* types = (typesList*)malloc(sizeof(typesList));
      linkedList* list = makeLink(NULL,NULL,NULL);

      if (! isVoid(tree->right->left,types) )
        getTypes(tree->right->left,types);

      type = tree->left->left->token;
      name = tree->left->right->token;

      declaration(type,name,types,currentScope);

      currentScope->right = makeScope(list,currentScope);
      currentScope = currentScope->right;

      currentScope -> returnType = type;

      decl_list(tree->right->left,currentScope);
  }

  char* semantizeExpression(node* tree, scope* currentScope) {

    if (isBoolean(tree->token)) return "boolean";
    if (isInt(tree->token)) return "int";
    
    if (strcmp(tree->token, "function call") == 0)
      return typeOf(tree->left->token,currentScope);

    return typeOf(tree->token,currentScope);
  }

  void semantizeCondition(node* tree, scope* currentScope) {
    char* isBool = semantizeExpression(tree, currentScope);

    if (strcmp(isBool,"boolean") != 0) {
      printf("Condition did not evalute to a boolean expression\n");
    }
  }

  void semantizeSameTypes(node* tree, scope* currentScope) {

    char* type = sameTypes(tree->token);
    char* type1 = semantizeExpression(tree->left,currentScope);
    char* type2 = semantizeExpression(tree->right,currentScope);

    if (!type1 || !type2) {
      printf("\nError: No type for %s\n", tree->token);
      return;
    }

    if (strcmp(type1,type2) != 0) {
      printf("\nError: <%s> %s <%s>\n",type1, tree->token, type2);
      return;
    }

    if (strcmp(type,"equality") != 0 && strcmp(type1,type) != 0) {
      printf("\nError: %s should be type %s\n", type1, type);
    }
  }

  void semantizeType(node* tree, scope* currentScope){
    char* types = unarType(tree->token);
    char* type1 = semantizeExpression(tree->left,currentScope);


    char* exactType = (char*) malloc(sizeof(type1) + 1 );
    strcpy(exactType,type1);
    strcat(exactType," ");

    if (!type1) { 
      printf("\nError no type for %s\n", tree->token);
      return;
    }

    
    if (
      !strstr(types,type1)
      ||
      strncmp(
          exactType,
          strstr(types,type1),
          strlen(exactType)
       )) {
      printf("\nError: %s should be type %s\n", type1, types);      
    }
  }

  void semantizeDifferentTypes(node* tree, scope* currentScope){
    char* types = differentTypes(tree->token);

    char* type1 = semantizeExpression(tree->right,currentScope);
    char* type2 = semantizeExpression(tree->left,currentScope);    


    char* exactType = (char*) malloc(sizeof(type2) + 1 );
    strcpy(exactType,type2);
    strcat(exactType," ");

    if (!type1 || !type2) { 
      printf("\nError no type for %s\n", tree->token);
      return;
    }

    if (strcmp(type1,"int") != 0) { 
      printf("%s must be used on type int\n", tree->token);
      return;
    }

    if (
      !strstr(types,type2)
      ||
      strncmp(
          exactType,
          strstr(types,type2),
          strlen(exactType)
       )) {
      printf("\nError: %s should be type %s\n", type2, types);      
    }
  }

  void semantizeReturnType(node* tree, scope* currentScope){
    char* funcType = currentScope->returnType;
    char* returnType;
    if (strcmp(tree-> token," ") == 0)
      returnType = "void";
    else
      returnType= semantizeExpression(tree,currentScope);

    if (strcmp(funcType,returnType) != 0) {
      printf("Return type '%s' must match function return type '%s'\n", returnType, funcType );
    }

  }

  void semantizeAssignment(node* tree, scope* currentScope) {
    char* type;
    char* assignType = semantizeExpression(tree->right,currentScope);

    if (strcmp(tree->left->token,"declaration") != 0) 
      type = typeOf(tree->left->token,currentScope);
    else
      type = tree->left->left->token;

    if (strcmp(type,assignType) != 0) {
      printf("Error: %s was assigned to type of %s\n", assignType, type );
    }
  }

  void semantizeFunctionCall(node* tree,scope* currentScope) {
    int countArgs = 0;

    char* funcName = tree->left->token;
    char *type1, *type2;

    typesList* typesList = typesOfInScope(funcName,currentScope);
    node* exprList = tree->right;

    printTree(exprList);

    if (strcmp(typesList->type,"void") == 0 ) {

      if(strcmp(exprList->token, " ") != 0 ) printf("Function %s should be called with no arguments\n",funcName);

      return;
    }

    while(typesList->type && exprList) {
      countArgs++;
      type1 = typesList->type;
      type2 = semantizeExpression(exprList->left,currentScope);

      if (strcmp(type1,type2) != 0) {
        printf("Function %s call not matching signature: Argument %d should be %s\n",
          funcName,countArgs,type1);
        return;
      }
      typesList = typesList->next;
      exprList = exprList->right;
    }

    if (typesList->type || exprList) {
      printf("FUCK YOU %d %d %d \n",countArgs,countArgs,countArgs);
    }


  }
  void semantizeTree(node* tree, scope* currentScope) {
    linkedList* list;
    typesList* types;
    char *name,*type,*token = tree->token;

    if (strcmp(token, "function def") == 0) {
      semantizeFunctionDef(tree,currentScope);
      currentScope = currentScope->right;
      tree = tree->right->right->left;
    }

    else if (strcmp(token, "function call") == 0) {
      semantizeFunctionCall(tree,currentScope);
    }
    else if ( strcmp(token,"decl list ") == 0) {
      semantizeDeclList(tree,currentScope);
      return;
    }

    else if ( strcmp(token,"declaration") == 0) {
      semantizeDeclaration(tree,currentScope);
      return;
    }

    else if ( differentTypes(token)) {
      semantizeDifferentTypes(tree,currentScope);
    }
    else if ( sameTypes(token) ) {
      semantizeSameTypes(tree,currentScope);
    }

    else if ( unarType(token)) {
      semantizeType(tree,currentScope);
    }

    else if( strcmp(token, "if") == 0) {
      semantizeCondition(tree->left, currentScope);
    }
    else if( strcmp(token, "while") == 0) {
      semantizeCondition(tree->left, currentScope);
    }
    else if( strcmp(token, "do while") == 0) {
      semantizeCondition(tree->left, currentScope);
    }
    else if( strcmp(token, "condition") == 0) {
      semantizeCondition(tree->left, currentScope);
    }
    else if( strcmp(token, "return") == 0) {
      semantizeReturnType(tree->left, currentScope);
    }
    else if( strcmp(token, "=") == 0) { 
      semantizeAssignment(tree,currentScope);
    }
    else if( isNotKeyword(token) && isIdent(token)) {
      semantizeIdentifier(tree,currentScope);
    }

    if ( tree->left ) {
      semantizeTree( tree->left , currentScope);
    }

    if ( tree->right ) {
      semantizeTree( tree->right, currentScope);
    }

    if(strcmp(token,"block") == 0) {
      // currentScope = currentScope-> left;
      // free(currentScope->right);
      // currentScope->right = NULL;
    }
  }