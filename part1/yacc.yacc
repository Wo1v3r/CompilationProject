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

%left   AND OR
%left   EQUALS GEQUALS LEQUALS GREATER LOWER NEQUALS
%left   PLUS MINUS 
%left   MULT DIV
%left   NOT
%%

s:    
      tree { printf("This is a tree\n"); printTree($1); freeTree($1); }

tree:
      comparison            |
      arith                 |
      LEFTPAR tree RIGHTPAR |
      NOT tree { $$ = makeNode("!", $2, NULL); }

comparison:
      comparison AND comparison { $$ = makeNode("&&", $1, $3); }    |
      comparison OR comparison  { $$ = makeNode("||", $1, $3); }    |
      arith EQUALS  arith       { $$ = makeNode("==", $1, $3); }    |
      arith GEQUALS arith       { $$ = makeNode(">=", $1, $3); }    |
      arith LEQUALS arith       { $$ = makeNode("<=", $1, $3); }    |
      arith GREATER arith       { $$ = makeNode(">",  $1, $3); }    |
      arith LOWER   arith       { $$ = makeNode("<",  $1, $3); }    |
      arith NEQUALS arith       { $$ = makeNode("!=", $1, $3); }    |
      arith

arith:
      NOT arith         { $$ = makeNode("!", $2, NULL);}         |
      arith PLUS  arith { $$ = makeNode("+", $1, $3); }         |
      arith MINUS arith { $$ = makeNode("-", $1, $3); }         |  
      arith MULT  arith { $$ = makeNode("*", $1, $3); }         |
      arith DIV   arith { $$ = makeNode("/", $1, $3); }         |

      NUM               { $$ = makeNode(yytext,NULL,NULL); }  |
      IDENTIFIER        { $$ = makeNode(yytext,NULL,NULL); }

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
