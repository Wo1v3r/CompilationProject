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
      : expr  SEMICOLON { $$ = $1; }
      | statement { $$ = $1; }


statement
      : if_statement
      | while_statement
      | do_while_statement SEMICOLON
      | for_statement

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


for_statement
      : FOR LEFTPAR for_kushalaimo RIGHTPAR block_statement { $$ = makeNode ("FOR", $3, $5) ; }

for_kushalaimo
      : for_expr SEMICOLON for_expr SEMICOLON for_expr
      { $$ = makeNode("FOR_KUSHALAIMO", $1, makeNode("FOR_KUSHALAIMO2", $3, $5)) ;}

for_expr
      : expr { $$ = $1; }
      | empty_expr { $$ = $1;}

// Recurse here on S >:
// block
//       : LEFTCURL expr RIGHTCURL { $$ = makeNode("{}",  $2, NULL); }

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

      | NUM                     { $$ = makeNode(yytext,NULL,NULL); }  
      | NULLVALUE               { $$ = makeNode(yytext,NULL,NULL); }
      | ident                   { $$ = $1; }

ident
      : IDENTIFIER              { $$ = makeNode(yytext,NULL,NULL); }

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

  void printTree(node* tree) {
    static int count = 0;

    count++;
    if (tree->left) {
      printTree(tree->left);
    }

    for (int i = 0 ; i < count ; i++){
      printf("\t");
    }

    printf("%s\n", tree->token);

    if (tree->right) {
      printTree(tree->right);
    }

    count--;
  }

  int main() {
    return yyparse();
  }

  int yyerror() {
    printf("MY ERROR\n");
    return 0;
  }
