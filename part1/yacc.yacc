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
%left   PLUS MINUS 
%left   MULT DIV
%%

s:    expr { printf("OK\n"); printTree($1); }

expr: 
      expr PLUS expr { $$ = makeNode("+", $1, $3); } |
      expr MINUS expr { $$ = makeNode("-", $1, $3); } | 
      expr MULT expr { $$ = makeNode("*", $1, $3); } |
      expr DIV expr { $$ = makeNode("/", $1, $3); } |

      NUM { $$ = makeNode(yytext,NULL,NULL); }
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

  void printTree(node* tree) {
    static count = 0;

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
