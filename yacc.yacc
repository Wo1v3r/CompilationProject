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

  void printTree(node* tree);
  void freeTree(node* tree);
  int yyerror(const char*);
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
      : tree { printf("Program\n=======\n\n"); printTree($1); freeTree($1); }

tree
      : line tree {  $$ = makeNode("line", $1 , $2);   }
      | line      {  $$ = makeNode("line", $1 , NULL); }

line
      : command
      | statement

command
      : expr SEMICOLON
      | SEMICOLON

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
      | func_type list_ident { $$ = makeNode("decl list ", $1, $2);  }
      | decl

function_call
      : ident LEFTPAR call_arguments RIGHTPAR { $$ = makeNode ("function call", $1, $3); }

function_statement
      : func_decl LEFTPAR function_arguments RIGHTPAR block_statement { $$ = makeNode("function def", $1, makeNode("settings", $3, $5)); }

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
      | RETURN expr             { $$ = makeNode("return", $2, NULL); }
      | RETURN                  { $$ = makeNode("return", NULL, NULL); }
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

      | REFERENCE ident         { $$ = makeNode("reference", $2, NULL); }
      | REFERENCE memory        { $$ = makeNode("reference", $2, NULL); }
      | POINTER ident           { $$ = makeNode("pointer", $2, NULL); }
      | function_call

      | ident                   
      | num
      
func_decl
      : func_type variable         { $$ = makeNode("declaration", $1, $2); }

decl
      : type variable              { $$ = makeNode("declaration", $1, $2); }
      | func_type variable         { $$ = makeNode("declaration", $1, $2); }

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
      : VOID        { $$ = makeNode(yytext,NULL,NULL); }
      | STRING      { $$ = makeNode(yytext,NULL,NULL); }

func_type
      : BOOL        { $$ = makeNode(yytext,NULL,NULL); }
      | CHARP       { $$ = makeNode(yytext,NULL,NULL); }
      | CHAR        { $$ = makeNode(yytext,NULL,NULL); }
      | INTP        { $$ = makeNode(yytext,NULL,NULL); }
      | INT         { $$ = makeNode(yytext,NULL,NULL); }

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

  int main() {
    return yyparse();
  }

  int yyerror(const char* error) {
    if(strcmp(error,"syntax error") != 0){
          fprintf(stderr, "Error in line: %d.\nScanner error: Unkown token '%s'\n", linenum, yytext);
          return 0;
    }
    fprintf(stderr, "Error in line: %d. '%s' was not expected.\n", linenum, yytext);
    return 0;
  }