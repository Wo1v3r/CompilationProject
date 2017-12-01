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

  int yyerror();
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
      : tree {printf("Program\n=======\n\n"); printTree($1); freeTree($1); }

tree
      : line tree {  $$ = makeNode("line", $1 , $2);   }
      | line      {  $$ = makeNode("line", $1 , NULL); }

line
      : expr  SEMICOLON { $$ = $1; }
      | statement { $$ = $1; }

statement
      : if_statement
      | while_statement
      | do_while_statement SEMICOLON
      | for_statement
      | function_statement
      | declaration_statement SEMICOLON

if_statement
      : IF wrapped_expr block_statement                      { $$ = makeNode("if", $2, makeNode("then, else", $3, NULL))  ;}
      | IF wrapped_expr block_statement ELSE block_statement { $$ = makeNode("if", $2, makeNode("then, else", $3, $5) )   ;}
      | IF wrapped_expr block_statement ELSE if_statement    { $$ = makeNode("if", $2, makeNode("then, else", $3, $5) )   ;}

while_statement
      : WHILE wrapped_expr block_statement { $$ = makeNode("while", $2, $3) ;}

do_while_statement
      : DO block_statement WHILE wrapped_expr { $$ = makeNode("do while", $4 , $2 ) ;}

block_statement
      : LEFTCURL tree RIGHTCURL { $$ = makeNode("block", $2, NULL); }

declaration_statement
      : decl ASSIGN ident index { $$ = makeNode("=", $1, makeNode("[]", $3, $4) ); } 
      | decl ASSIGN expr  { $$ = makeNode("=", $1, $3); }
      | type list_ident { $$ = makeNode("decl list ", $1, $2);  }
      | decl  {$$ = $1;}

function_call
      : ident LEFTPAR call_arguments RIGHTPAR { $$ = makeNode ("function call", $1, $3); }

function_statement  // This is equivalent to a function call, not a definition
      : decl LEFTPAR function_arguments RIGHTPAR block_statement { $$ = makeNode("function def", $1, makeNode("settings", $3, $5)); }
      | decl LEFTPAR function_arguments RIGHTPAR SEMICOLON { $$ = makeNode("function dec", $1, $3); }

function_arguments
      : empty_expr  // This is not good because function1(())
      | list_dec { $$ = $1; }

call_arguments
      : empty_expr
      | list_expr { $$ = $1; }

list_dec
      : decl { $$ = makeNode("dec", $1 , NULL); }
      | decl COMMA list_dec { $$ = makeNode("dec", $1, $3); }

list_ident
      : ident { $$ = makeNode("ident", $1, NULL); }
      | ident COMMA list_ident { $$ = makeNode("ident", $1, $3); }

list_expr
      : expr { $$ = makeNode("exp", $1 , NULL); }
      | expr COMMA list_expr {$$ = makeNode("exp", $1, $3); }



for_statement
      : FOR LEFTPAR for_kushalaimo RIGHTPAR block_statement { $$ = makeNode ("for", $3, $5) ; }

for_kushalaimo
      : for_expr SEMICOLON for_expr SEMICOLON for_expr
      { $$ = makeNode("for_kushalaimo", $1, makeNode("for_kushalaimo2", $3, $5)) ;}

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
      | RETURN expr             { $$ = makeNode("return", $2, NULL); }
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
      | REFERENCE ident         { $$ = makeNode("reference", $2, NULL); }
      | POINTER ident           { $$ = makeNode("pointer", $2, NULL); }

decl
      : type ident              { $$ = makeNode("declaration", $1, $2); }

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

  void tab(int times) {
    printf("\n");

    for (int i = 0 ; i < times ; i++) {
      printf("\t");
    }
  }

  int shouldTab(char* token) {
    if (
      strcmp(token,"line") == 0
      ||
      strcmp(token,"then, else") == 0
      ) {
      return 0;
    }
    return 1;
  }

  void printTree(node* tree) {

    static int tabCount = 0;
    char* token = tree->token;

    if ( shouldTab(token) ) {
      printf("%s", token);
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
      tab(tabCount);
    }
  }

  int main() {
    return yyparse();
  }

  int yyerror() {
    printf("Some error\n");
    return 0;
  }