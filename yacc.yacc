%{
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include <ctype.h>
  #include <regex.h>
  #include "lex.yy.c"

  typedef struct node {
    char* token;
    struct node* left;
    struct node* right;
  } node;

  typedef struct typesList {
    char* type;
    struct typesList* next;
  } typesList;

  typedef struct linkedList {
    char* name;
    char* type;
    typesList* types;
    struct linkedList* next;
  } linkedList;

  typedef struct scope {
    struct linkedList* list;
    struct linkedList* current;
    struct scope* left;
    struct scope* right;
  } scope;


  node* makeNode(char* token, node* left, node* right);
  linkedList* makeLink(char* token,char* type, linkedList* next);  
  scope* makeScope(linkedList* list, scope* left);

  void buildTree(node* tree);

  void semantizeTree(node* tree, scope* currentScope);
  void freeTree(node* tree);

  void printTree(node* tree);
  void printScope(scope* scope);
  void printList(linkedList* list);
  
  int yyerror(const char*);
  #define YYSTYPE struct node*

%}

%token  NUM NULLVALUE
%token  MULT PLUS MINUS DIV
%token  POINTER REFERENCE ASSIGN
%token  AND OR NOT
%token  EQUALS GEQUALS LEQUALS GREATER LOWER NEQUALS
%token  BOOL CHARP CHAR VOID INTP INT STRING
%token  B_TRUE B_FALSE
%token  IF ELSE WHILE DO FOR RETURN
%token  LEFTPAR LEFTBRAC LEFTCURL
%token  RIGHTPAR RIGHTBRAC RIGHTCURL
%token  COMMA SEMICOLON
%token  PIPE
%token  IDENTIFIER
%token  STRINGVALUE CHARVALUE

%start  program

%left   RETURN
%left   ASSIGN
%left   AND OR
%left   EQUALS GEQUALS LEQUALS GREATER LOWER NEQUALS
%left   PLUS MINUS 
%left   MULT DIV
%left   NOT

%%

program
      : tree { printf("Program\n=======\n\n"); buildTree($1); freeTree($1); }

tree
      : line tree {  $$ = makeNode("line", $1 , $2);   }
      | line      {  $$ = makeNode("line", $1 , NULL); }

line
      : command
      | statement

command
      : expr SEMICOLON
      | RETURN return_statement SEMICOLON { $$ = makeNode("return", $2, NULL); }
      | SEMICOLON

return_statement
      : expr
      | empty_expr

statement
      : if_statement
      | while_statement
      | do_while_statement SEMICOLON
      | for_statement
      | function_statement
      | declaration_statement SEMICOLON

if_statement
      : IF wrapped_expr command_statement                         { $$ = makeNode("if", $2, makeNode("then, else", $3, NULL))  ;}
      | IF wrapped_expr command_statement ELSE command_statement  { $$ = makeNode("if", $2, makeNode("then, else", $3, $5) )   ;}
      | IF wrapped_expr command_statement ELSE if_statement       { $$ = makeNode("if", $2, makeNode("then, else", $3, $5) )   ;}

command_statement
      : block_statement
      | command

while_statement
      : WHILE wrapped_expr command_statement { $$ = makeNode("while", $2, $3) ;}

do_while_statement
      : DO block_statement WHILE wrapped_expr { $$ = makeNode("do while", $4 , $2 ) ;}

block_statement
      : LEFTCURL tree RIGHTCURL { $$ = makeNode("block", $2, NULL); }

declaration_statement
      : decl ASSIGN expr  { $$ = makeNode("=", $1, $3); }
      | type list_ident { $$ = makeNode("decl list ", $1, $2);  }

function_call
      : ident LEFTPAR call_arguments RIGHTPAR { $$ = makeNode ("function call", $1, $3); }

function_statement
      : decl LEFTPAR function_arguments RIGHTPAR block_statement { $$ = makeNode("function def", $1, makeNode("settings", $3, $5)); }

function_arguments
      : empty_expr
      | list_dec

call_arguments
      : empty_expr
      | list_expr

list_dec
      : decl { $$ = makeNode("dec_list", $1 , NULL); }
      | decl COMMA list_dec { $$ = makeNode("dec_list", $1, $3); }

list_ident
      : variable { $$ = makeNode("ident_list", $1, NULL); }
      | variable COMMA list_ident { $$ = makeNode("ident_list", $1, $3); }

list_expr
      : expr { $$ = makeNode("expr_list", $1 , NULL); }
      | expr COMMA list_expr {$$ = makeNode("expr_list", $1, $3); }


for_statement
      : FOR LEFTPAR for_setup RIGHTPAR command_statement { $$ = makeNode ("for", $3, $5) ; }

for_setup
      : for_init SEMICOLON for_condition SEMICOLON for_update
      { $$ = makeNode("setup", $1, makeNode("inner", $3, $5)) ;}

for_init
      : for_expr { $$ = makeNode("init", $1, NULL); }
for_condition
      : for_expr { $$ = makeNode("condition", $1, NULL); }
for_update
      : for_expr { $$ = makeNode("update", $1, NULL); }

for_expr
      : expr
      | empty_expr

empty_expr
      :  { $$ = makeNode(" ", NULL, NULL); }
      |  LEFTPAR empty_expr RIGHTPAR { $$ = $2; }

wrapped_expr
      : LEFTPAR expr RIGHTPAR   { $$ = $2; }
expr
      : wrapped_expr
      | NOT  expr               { $$ = makeNode("!",  $2, NULL); }
      | PIPE expr PIPE          { $$ = makeNode("abs", $2, NULL); }
      | expr AND expr           { $$ = makeNode("&&", $1, $3); }        
      | expr OR  expr           { $$ = makeNode("||", $1, $3); }        
      | expr EQUALS  expr       { $$ = makeNode("==", $1, $3); }        
      | expr GEQUALS expr       { $$ = makeNode(">=", $1, $3); }        
      | expr LEQUALS expr       { $$ = makeNode("<=", $1, $3); }        
      | expr GREATER expr       { $$ = makeNode(">",  $1, $3); }        
      | expr LOWER   expr       { $$ = makeNode("<",  $1, $3); }        
      | expr NEQUALS expr       { $$ = makeNode("!=", $1, $3); }        
      | expr PLUS  expr         { $$ = makeNode("+", $1, $3); }         
      | expr MINUS expr         { $$ = makeNode("-", $1, $3); }         
      | expr MULT  expr         { $$ = makeNode("*", $1, $3); }         
      | expr DIV   expr         { $$ = makeNode("/", $1, $3); }

      | ident ASSIGN expr       { $$ = makeNode("=", $1, $3); }
      | memory ASSIGN expr      { $$ = makeNode("=", $1, $2); }  
      | memory


      | STRINGVALUE             { $$ = makeNode(yytext, NULL , NULL); }
      | CHARVALUE               { $$ = makeNode(yytext, NULL , NULL); }
      | NULLVALUE               { $$ = makeNode(yytext,NULL,NULL); }
      | B_FALSE                 { $$ = makeNode(yytext,NULL,NULL); }
      | B_TRUE                  { $$ = makeNode(yytext,NULL,NULL); }

      | REFERENCE ident         { $$ = makeNode("reference", $2, NULL); }
      | REFERENCE memory        { $$ = makeNode("reference", $2, NULL); }
      | POINTER ident           { $$ = makeNode("pointer", $2, NULL); }
      | function_call

      | ident                   
      | num

decl
      : type variable              { $$ = makeNode("declaration", $1, $2); }

variable
      : ident
      | ident index       { $$ = makeNode("array", $1, $2 ); }

memory
      : ident access_index      { $$ = makeNode("[]", $1, $2); }

access_index
      : LEFTBRAC expr RIGHTBRAC { $$ = $2; }

index
      : LEFTBRAC num RIGHTBRAC { $$ = $2; }

type
      : BOOL        { $$ = makeNode(yytext,NULL,NULL); }
      | STRING      { $$ = makeNode(yytext,NULL,NULL); }
      | CHARP       { $$ = makeNode(yytext,NULL,NULL); }
      | CHAR        { $$ = makeNode(yytext,NULL,NULL); }
      | INTP        { $$ = makeNode(yytext,NULL,NULL); }
      | INT         { $$ = makeNode(yytext,NULL,NULL); }
      | VOID        { $$ = makeNode(yytext,NULL,NULL); }

ident
      : IDENTIFIER  { $$ = makeNode(yytext,NULL,NULL); }

num   
      : NUM         { $$ = makeNode(yytext,NULL,NULL); }  

%%
  node* makeNode(char* token, node* left, node* right) {
    node* newNode = (node*)malloc(sizeof(node));
    char* newStr = (char*)malloc(sizeof(token)+1);
    strcpy(newStr,token);
    newNode -> left = left;
    newNode -> right = right;
    newNode -> token = newStr;
    return newNode;
  }

  linkedList* makeLink(char* token, char* type, linkedList* next) {
    linkedList* newLink = (linkedList*)malloc(sizeof(linkedList));

    newLink -> name = token;
    newLink -> type = type;
    newLink -> next = NULL;

    return newLink;
  }

  scope* makeScope(linkedList* list, scope* left) {
    scope* newScope = (scope*)malloc(sizeof(scope));

    newScope -> list = list;
    newScope -> current = list;
    newScope -> left = left;
    newScope -> right = NULL;

    return newScope;
  }


  void freeTree(node* tree){
      if (tree != NULL) {
        freeTree(tree->right);
        free(tree->token);
        freeTree(tree->left);
        free(tree);
    }
  }

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

  void printScope(scope* scope) {
    int count = 0;
    while (scope) {
      printf("Scope#%d\n",count++);
      if (scope->list) printList(scope->list);
      scope = scope->right;
    }
  }

  void printList(linkedList* list){
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

  void buildTree(node* tree){
    printf("AST:\n\n");

    printTree(tree);
    linkedList*  globalList = makeLink(NULL,NULL,NULL);
    scope* globalScope= makeScope(globalList,NULL);

    printf("Semantics:\n\n");
    semantizeTree(tree, globalScope);

    printScope(globalScope);
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
    int length = 9;

    char* keywords[] = {
      "||", "&&","==","!=","!","<","<=",">",">="
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
      int length = 8;

      char* keywords[] = {
        ">", "<", ">=", "<=", "*", "/", "+", "-"
      };

      for (int i = 0 ; i < length ; i++ ) {
        if (strcmp(token,keywords[i]) == 0) 
          return 1;
      }

      return 0;
  }

  void semantizeBlock(scope* currentScope) {
      linkedList* list = makeLink(NULL,NULL,NULL);
      currentScope->right = makeScope(list,currentScope);
      currentScope = currentScope->right;
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

      decl_list(tree->right->left,currentScope);
  }

  char* semantizeExpression(node* tree, scope* currentScope) {

    if (isBoolean(tree->token)) return "boolean";
    if (isInt(tree->token)) return "int";
    
    if (strcmp(tree->token, "function call") == 0)
      return typeOf(tree->left->token,currentScope);

    return typeOf(tree->token,currentScope);
  }


  void semantizeOperator(node* tree, scope* currentScope) {
    char* type1 = semantizeExpression(tree->left,currentScope);
    char* type2 = semantizeExpression(tree->right,currentScope);

    if (!type1 || !type2) {
      printf("\nError: No type for %s\n", tree->token);
      return;
    }

    if (strcmp(type1,type2) != 0) {
      printf("\nError: <%s> %s <%s>\n",type1, tree->token, type2);
    }
  }

  void semantizeTree(node* tree, scope* currentScope) {
    linkedList* list;
    typesList* types;
    char *name,*type,*token = tree->token;

    if (strcmp(token, "block") == 0) {
      semantizeBlock(currentScope);
    }

    else if (strcmp(token, "function def") == 0) {
      semantizeFunctionDef(tree,currentScope);
      currentScope = currentScope->right;
      tree = tree->right->right->left;
    }

    else if ( strcmp(token,"decl list ") == 0) {
      semantizeDeclList(tree,currentScope);
      return;
    }

    else if ( strcmp(token,"declaration") == 0) {
      semantizeDeclaration(tree,currentScope);
      return;
    }

    else if (boolBool(token) || intInt(token) || equality(token)) {
      semantizeOperator(tree,currentScope);
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
  

  int main() {
    return yyparse();
  }

  int yyerror(const char* error) {
    if(strcmp(error,"syntax error") != 0){
          fprintf(stderr, "Error in line: %d.\nScanner error: Unknown token '%s'\n", linenum, yytext);
          return 0;
    }
    fprintf(stderr, "Error in line: %d. '%s' was not expected.\n", linenum, yytext);
    return 0;
  }