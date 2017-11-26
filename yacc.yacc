%{
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include "lex.yy.c"

  typedef struct node {
    char* token;
    struct node* left;
    struct node* right;
  } node;

  node* makeNode(char* token, node* left, node* right);

  char* getAction(int index, int action);
  int getIndex(char* token);
  void printTree(node* tree);
  void freeTree(node* tree);
  #define YYSTYPE struct node*
%}

%token  NUM NULLVALUE
%token  MULT PLUS MINUS DIV
%token  POINTER REFERENCE ASSIGN
%token  AND OR NOT
%token  EQUALS GEQUALS LEQUALS GREATER LOWER NEQUALS
%token  BOOL CHARP CHAR VOID INTP INT STRING
%token  IF ELSE WHILE DO FOR RETURN
%token  LEFTPAR LEFTBRAC LEFTCURL
%token  RIGHTPAR RIGHTBRAC RIGHTCURL
%token  COMMA SEMICOLON
%token  PIPE
%token  IDENTIFIER

%start  program

%left   AND OR
%left   EQUALS GEQUALS LEQUALS GREATER LOWER NEQUALS
%left   PLUS MINUS 
%left   MULT DIV
%left   NOT
%%

program
      : tree {printf("THIS IS THE PROGRAM\n"); printTree($1); freeTree($1); }

tree
      : line tree {  $$ = makeNode("Line", $1 , $2);   }
      | line      {  $$ = makeNode("Line", $1 , NULL); }

line
      : RETURN line { $$ = makeNode("Return", $2, NULL); }
      | expr  SEMICOLON { $$ = $1; }
      | statement { $$ = $1; }

statement
      : if_statement
      | while_statement
      | do_while_statement SEMICOLON
      | for_statement
      | function_statement
      | declaration_statement SEMICOLON

if_statement
      : IF wrapped_expr block_statement                      { $$ = makeNode("IF", $2, makeNode("THEN,ELSE", $3, NULL))  ;}
      | IF wrapped_expr block_statement ELSE block_statement { $$ = makeNode("IF", $2, makeNode("THEN,ELSE", $3, $5) )   ;}
      | IF wrapped_expr block_statement ELSE if_statement    { $$ = makeNode("IF", $2, makeNode("THEN,ELSE", $3, $5) )   ;}

while_statement
      : WHILE wrapped_expr block_statement { $$ = makeNode("WHILE", $2, $3) ;}

do_while_statement
      : DO block_statement WHILE wrapped_expr { $$ = makeNode("DO WHILE", $4 , $2 ) ;}

block_statement
      : LEFTCURL tree RIGHTCURL { $$ = makeNode("Block", $2, NULL); }

declaration_statement
      : decl ASSIGN ident index { $$ = makeNode("=", $1, makeNode("[]", $3, $4) ); } 
      | decl ASSIGN expr  { $$ = makeNode("=", $1, $3); }
      | type list_ident { $$ = makeNode("Declaration List", $1, $2);  }
      | decl  {$$ = $1;}

function_call
      : ident LEFTPAR call_arguments RIGHTPAR { $$ = makeNode ("Function call", $1, $3); }

function_statement  // This is equivalent to a function call, not a definition
      : decl LEFTPAR function_arguments RIGHTPAR block_statement { $$ = makeNode("Function def", $1, makeNode("Settings", $3, $5)); }
      | decl LEFTPAR function_arguments RIGHTPAR SEMICOLON { $$ = makeNode("Function dec", $1, $3); }

function_arguments
      : empty_expr  // This is not good because function1(())
      | list_dec { $$ = $1; }

call_arguments
      : empty_expr
      | list_expr { $$ = $1; }

list_dec
      : decl { $$ = makeNode("Declarations", $1 , NULL); }
      | decl COMMA list_dec { $$ = makeNode("Declarations", $1, $3); }

list_ident
      : ident { $$ = makeNode("Identifiers", $1, NULL); }
      | ident COMMA list_ident { $$ = makeNode("Identifiers", $1, $3); }

list_expr
      : expr { $$ = makeNode("Expressions", $1 , NULL); }
      | expr COMMA list_expr {$$ = makeNode("Expressions", $1, $3); }



for_statement
      : FOR LEFTPAR for_kushalaimo RIGHTPAR block_statement { $$ = makeNode ("FOR", $3, $5) ; }

for_kushalaimo
      : for_expr SEMICOLON for_expr SEMICOLON for_expr
      { $$ = makeNode("FOR_KUSHALAIMO", $1, makeNode("FOR_KUSHALAIMO2", $3, $5)) ;}

for_expr
      : expr { $$ = $1; }
      | empty_expr { $$ = $1;}

empty_expr
      :  { $$ = makeNode(" ", NULL, NULL); }
      |  LEFTPAR empty_expr RIGHTPAR { $$ = $2; }

wrapped_expr
      : LEFTPAR expr RIGHTPAR   { $$ = $2; }
expr
      : wrapped_expr   { $$ = $1; }
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
      | function_call           { $$ = $1; }

      | NUM                     { $$ = makeNode(yytext,NULL,NULL); }  
      | NULLVALUE               { $$ = makeNode(yytext,NULL,NULL); }
      | ident                   { $$ = $1; }
      | REFERENCE ident         { $$ = makeNode("Reference", $2, NULL); }
      | POINTER ident           { $$ = makeNode("Pointer", $2, NULL); }

decl
      : type ident              { $$ = makeNode("Declaration", $1, $2); }

index
      : LEFTBRAC expr RIGHTBRAC { $$ = $2; }

type
      : BOOL        { $$ = makeNode(yytext,NULL,NULL); }
      | CHARP       { $$ = makeNode(yytext,NULL,NULL); }
      | CHAR        { $$ = makeNode(yytext,NULL,NULL); }
      | VOID        { $$ = makeNode(yytext,NULL,NULL); }
      | INTP        { $$ = makeNode(yytext,NULL,NULL); }
      | INT         { $$ = makeNode(yytext,NULL,NULL); }
      | STRING      { $$ = makeNode(yytext,NULL,NULL); }

ident
      : IDENTIFIER  { $$ = makeNode(yytext,NULL,NULL); }

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

  void freeTree(node* tree){
      if (tree != NULL) {
        freeTree(tree->right);
        free(tree->token);
        freeTree(tree->left);
        free(tree);
    }
  }

  char* getAction(index, action) {

    if (index == -1) return "";

    char* actions[4][3] = {
      {"\n","\n","\n"},
      {"\n","\n","\n"},
      {"\n","\n","\n"},
      {"\n","\n","\n"}
    };

    return actions[index][action];
  }

  int getIndex(char* token) {
    int length = 4;
    char* tokens[] = {
      "Line",
      "WHILE",
      "Return",
      "Block"
    };

    for (int i = 0 ; i < length ; i++){
      if (strcmp(tokens[i],token) == 0) {
        return i;
      }
    }
    return -1;
  }

  void printTree(node* tree) {

    static int tabCount = 0;
    char* token = tree->token;
    int tokenIndex = getIndex(token);

    char* before = getAction(tokenIndex,0);
    char* middle = getAction(tokenIndex,1);
    char* after  = getAction(tokenIndex,2);
    
    
    printf("%s", before);

    if ( tree->left ) {
      printTree( tree->left );
    }

    printf("%s", middle);

    if( token) {
      printf("%s\n", token);
    }

    if ( tree->right ) {
      printTree( tree->right );
    }

    printf("%s", after);
  }

  int main() {
    return yyparse();
  }

  int yyerror() {
    printf("MY ERROR\n");
    return 0;
  }