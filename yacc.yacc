%{
  #include "lex.yy.c"
  #include <stdio.h>
  #include <string.h>
  #include <ctype.h>
  #include <regex.h>
  #include "generator.c"

  void buildTree(node* tree);
  void semantizeTree(node* tree, scope* currentScope);
  void freeTree(node* tree);
  // extern char* yytext;
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
      : tree { printf("Program\n=======\n\n"); buildTree($1); freeTree($1);}

tree
      : line tree {  $$ = makeNode("line", $1 , $2);   }
      | line      {  $$ = makeNode("line", $1 , NULL); }

line
      : command
      | statement
      | block_line

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

block_line
      : LEFTCURL RIGHTCURL { $$ = makeNode("block", NULL,NULL);}
      | block_statement

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
      | memory ASSIGN expr      { $$ = makeNode("=", $1, $3); }  
      | point ASSIGN expr       { $$ = makeNode("=", $1, $3); } 
      | refer ASSIGN expr   { $$ = makeNode("=", $1, $3); } 
      
      | memory


      | STRINGVALUE             { $$ = makeNode(yytext, NULL , NULL); }
      | CHARVALUE               { $$ = makeNode(yytext, NULL , NULL); }
      | NULLVALUE               { $$ = makeNode(yytext,NULL,NULL); }
      | B_FALSE                 { $$ = makeNode(yytext,NULL,NULL); }
      | B_TRUE                  { $$ = makeNode(yytext,NULL,NULL); }

      | REFERENCE memory        { $$ = makeNode("reference", $2, NULL); }
      | point
      | refer
      | function_call

      | ident                   
      | num

refer
      : REFERENCE ident { $$ = makeNode("reference", $2, NULL); }
point
      : POINTER ident           { $$ = makeNode("pointer", $2, NULL); }
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
  // #include "lex.yy.c"
  void buildTree(node* tree){
    printf("\n\nAST:\n\n");
    printTree(tree);


    printf("\n\nSemantics:\n\n");
    linkedList* globalList = makeLink(NULL,NULL,NULL);
    scope* globalScope= makeScope(globalList,NULL);
    semantizeTree(tree, globalScope);
    mainExists();

    if (errorFlag) {
      printf("Semantics Failed! exiting...\n\n");
      exit(1);
    }

    printf("\n\nLabelizer:\n\n");
    addLabels(tree);
    printTreeWithLabels(tree);

    printf("\n\nGenerator:\n\n");
    generateCode(tree);
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