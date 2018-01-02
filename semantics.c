  #include "print.c"
  int mainFlag = 0;
  int isChar(char* token);
  int isString(char* token);
  
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

    if (isString(token) || isChar(token)) return 0;
    
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

    if (strcmp(token,"[]") == 0) return "char";
    if (strcmp(token,"null") == 0) return "null";

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

  int typesListLength(typesList* list) {
    if (!list->next) return 0;
    
    return 1 + typesListLength(list->next);
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

      if (strcmp(name, "array") == 0 ) {
        name = identList->left->left->token;
      }

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
    types -> next -> type = NULL;
    types -> next -> next = NULL;

    return getTypes(tree->right, types -> next);
  }

  char* pointerOf(char* type){
    if (strcmp(type,"int") == 0) return "intp";
    if (strcmp(type,"char") == 0) return "charp";

    return NULL;
  }

  char* referenceOf(char* type){
    if (strcmp(type,"intp") == 0) return "int";
    if (strcmp(type,"charp") == 0) return "char";

    return NULL;
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

  int isString(char* token) {
    if (token && token[0] == '"' && token[strlen(token) - 1] == '"') 
      return 1;

    return 0;
  }

  int isChar(char* token){

    if (strcmp(token,"[]") == 0) 
      return 1;

    if (token && token[0] == '\'' && token[2] == '\'') 
      return 1;

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
    if (strcmp (token, "reference") == 0) return "int | char ";

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
      if (!typeOf(tree->token,currentScope)) 
        printf("Error: %s was used but not defined\n",tree->token);
  }

  int checkMain(char* token, typesList* types) {
    if (strcmp(token,"main") == 0) {
      if (strcmp(types->type, "void") != 0) {
        printf("Main function cannot have any arguments\n");
        return 1;
      }
      if (mainFlag) {
        printf("Main function can be defined only once in a program\n");
        return 1;
      }
      mainFlag = 1;
    }

    return 0;
  }

  void semantizeFunctionDef(node* tree, scope* currentScope){
      char *name, *type;
      typesList* types = (typesList*)malloc(sizeof(typesList));
      linkedList* list = makeLink(NULL,NULL,NULL);

      if (! isVoid(tree->right->left,types) )
        getTypes(tree->right->left,types);

      type = tree->left->left->token;
      name = tree->left->right->token;

      checkMain(name,types);
      declaration(type,name,types,currentScope);

      currentScope->right = makeScope(list,currentScope);
      currentScope->right-> returnType = type;
      decl_list(tree->right->left,currentScope->right);
  }


  int isNull(char* token){    
    if (strcmp(token,"null") == 0) return 1; 

    return 0;
  }

  int checkNull(char* actualType, char* token){

    if (!isNull(token)) return 1;

    if (strcmp(actualType,"intp") == 0) return 0;
    if (strcmp(actualType,"charp") == 0) return 0;

    printf("Error: null can be assigned only to intp | charp\n");
    return 0;
  }

  char* semantizeExpression(node* tree, scope* currentScope) {
    char* type;

    if (isBoolean(tree->token)) return "boolean";
    if (isInt(tree->token)) return "int";
    if (isString(tree->token)) return "string";
    if (isChar(tree->token)) return "char";
    
    if (strcmp(tree->token, "function call") == 0) {
      return typeOf(tree->left->token,currentScope);
    }

    if (strcmp(tree->token, "reference") == 0){
      type = typeOf(tree->left->token,currentScope);
      return pointerOf(type);
    }

    if (strcmp(tree->token, "pointer") == 0){
      type = typeOf(tree->left->token,currentScope);
      return referenceOf(type);
    }

    return typeOf(tree->token,currentScope);

  }

  void semantizeCondition(node* tree, scope* currentScope) {
    char* isBool = semantizeExpression(tree, currentScope);

    if (strcmp(isBool,"boolean") != 0) {
      printf("Error: condition must be boolean expression\n");
    }
  }

  void semantizeSameTypes(node* tree, scope* currentScope) {
    char* type = sameTypes(tree->token);
    char* type1 = semantizeExpression(tree->left,currentScope);
    char* type2 = semantizeExpression(tree->right,currentScope);


    if (
        (isNull(tree->right->token) && isNull(tree->left->token)) ||
        !checkNull(type1,tree->right->token) ||
        !checkNull(type2,tree->left->token)
        ){ 
      return;
    }
    if (type1 && type2 && strcmp(type1,type2) != 0) {
      printf("Error: <%s> %s <%s>\n",type1, tree->token, type2);
      return;
    }

    if (strcmp(type,"equality") != 0 && strcmp(type1,type) != 0) {
      printf("Error: %s should be type %s\n", type1, type);
    }
  }

  void semantizeType(node* tree, scope* currentScope){
    char* types = unarType(tree->token);
    char* type1 = semantizeExpression(tree->left,currentScope);


    char* exactType = (char*) malloc(sizeof(type1) + 1 );
    strcpy(exactType,type1);
    strcat(exactType," ");

    if (!type1) { 
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
      printf("Error: %s should be type %s\n", type1, types);      
    }
  }

  void semantizeDifferentTypes(node* tree, scope* currentScope){
    char* types = differentTypes(tree->token);

    char* type1 = semantizeExpression(tree->right,currentScope);
    char* type2 = semantizeExpression(tree->left,currentScope);

    if (!type1 || !type2) { 
      return;
    }
    
    char* exactType = (char*) malloc(sizeof(type2) + 1 );
    strcpy(exactType,type2);
    strcat(exactType," ");

    if (strcmp(type1,"int") != 0) { 
      printf("Error: %s must be used on type int\n", tree->token);
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
      printf("Error: %s should be type %s\n", type2, types);      
    }
  }

  void semantizeReturnType(node* tree, scope* currentScope){
    char* funcType = currentScope->left->returnType;

    if (!funcType){
      printf("Error: return statement must be in a function\n");
      return;
    }

    if (strcmp(funcType, "string") == 0) {
      printf("Error: function cannot return a string\n");
      return;
    }

    char* returnType;
    if (strcmp(tree-> token," ") == 0)
      returnType = "void";
    else
      returnType= semantizeExpression(tree,currentScope);

    if (returnType == NULL) return;
    if (checkNull(funcType,tree->token) && strcmp(funcType,returnType) != 0) {
      printf("Error: return type '%s' must match function return type '%s'\n", returnType, funcType );
    }

  }

  void semantizeAssignment(node* tree, scope* currentScope) {    
    char* type;
    char* assignType = semantizeExpression(tree->right,currentScope);

    if (strcmp(tree->left->token,"declaration") == 0)
      type = tree->left->left->token;
    else if (strcmp(tree->left->token,"[]") == 0)
      type = "char";
    else
      type = typeOf(tree->left->token,currentScope);

    if (!type || !assignType) return;

    if (checkNull(type,tree->right->token) && strcmp(type,assignType) != 0) {
      printf("Error: %s was assigned to type of %s\n", assignType, type );
    }
  }

  void semantizeIndex(node* tree, scope* currentScope) {
    char* type1;
    char* type2;

    type1 = semantizeExpression(tree->left, currentScope);
    type2 = semantizeExpression(tree->right, currentScope);

    if ( strcmp(type1, "string") != 0 ) {
      printf("Error: %s can not be accessed with an index\n", type1);
    }

    if ( strcmp(type2, "int") != 0 ) {
      printf("Error: Index can only be an integer\n");
    }

  }

  void semantizeFunctionCall(node* tree,scope* currentScope) {
    int countArgs = 0;
    int neededLength;
    char* funcName = tree->left->token;
    char *type1, *type2;


    typesList* typesList = typesOf(funcName,currentScope);

    if (!typesList) {
      printf("Error: %s is not a function!\n", funcName);
      return;
    }

    neededLength = typesListLength(typesList);

    node* exprList = tree->right;

    if (strcmp(typesList->type,"void") == 0 ){
      if (strcmp(exprList->token, " ") != 0 ) {
        printf("Error: Function %s should be called with no arguments\n",funcName);
        return;
      }
      return;
    }


    while(typesList->type && exprList) {
      type1 = typesList->type;
      
      if (exprList-> left)
        type2 = semantizeExpression(exprList->left,currentScope);
      else 
        break;

      if (!type2) {
        printf("Error: %s was used but not defined\n", exprList->left->token);
        exit(1);
      }

      if (checkNull(type1,exprList->left->token) && strcmp(type1,type2) != 0) {
        printf("Error: function %s call not matching signature: Argument %d should be %s\n",
          funcName,countArgs,type1);
        return;
      }
      typesList = typesList->next;
      exprList = exprList->right;
      countArgs++;
    }



    if (typesList->type || exprList) {
      while(exprList){
        exprList = exprList->right;
        if(exprList) countArgs++;
      }
      printf("Error: function '%s' was called with %d arguments instead of %d\n",funcName, countArgs, neededLength);
    }

  }
  void mainExists() { 
    if (!mainFlag) printf("Error: main function was not defined\n");
  }

  void semantizeTree(node* tree, scope* currentScope) {
    linkedList* list;
    typesList* types;
    char *name,*type,*token = tree->token;

    if (strcmp(token, "function def") == 0) {
      semantizeFunctionDef(tree,currentScope);
      currentScope = currentScope->right;
      semantizeTree(tree->right->right,currentScope);
      return;
    }

    else if(strcmp(token,"block") == 0) {
      linkedList* list = makeLink(NULL,NULL,NULL);
      currentScope->right = makeScope(list,currentScope);
      currentScope = currentScope->right;
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
    else if (strcmp(token, "[]") == 0) {
      semantizeIndex(tree,currentScope);
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

    if(strcmp(token,"block") == 0 || strcmp(token,"function def") == 0) {
      freeScope(currentScope);
      currentScope = NULL;
    }
  }
