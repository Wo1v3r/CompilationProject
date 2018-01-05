%{ 
  #include <stdio.h>
  void comment(void);
  int yyerror(const char*);
  int linenum = 1;
%}

%%
"//".*                ;

"*"        return MULT;//
"+"        return PLUS;//
"-"        return MINUS;// 
"/"        return DIV;//
"^"        return POINTER;//

"&&"      return AND;
"&"       return REFERENCE;//
"||"      return OR;
"=="      return EQUALS;
">="      return GEQUALS;
"<="      return LEQUALS;
">"       return GREATER;
"<"       return LOWER;

"!="      return NEQUALS;
"!"       return NOT;
"="       return ASSIGN;

"boolean" return BOOL;    
"charp"   return CHARP;   
"char"    return CHAR;    
"void"    return VOID;    
"intp"    return INTP;    
"int"     return INT;     
"string"  return STRING;  

"if"      return IF;     
"else"    return ELSE;   
"while"   return WHILE;  
"do"      return DO;     
"for"     return FOR;    
"return"  return RETURN; 

"false"   return B_FALSE;
"true"    return B_TRUE;

"null"    return NULLVALUE; 
[0-9]+    return NUM;       
(0x|0X)[0-9A-F]+ return NUM;
[0-1]+b   return NUM;

";"       return SEMICOLON; 
"("       return LEFTPAR;   
")"       return RIGHTPAR;  
"["       return LEFTBRAC;  
"]"       return RIGHTBRAC; 
"{"       return LEFTCURL;  
"}"       return RIGHTCURL; 
"|"       return PIPE;      
","       return COMMA;     


\"(\\.|[^"\\])*\"             return STRINGVALUE;
\'.\'                         return CHARVALUE;

[a-zA-Z_]([a-zA-Z_]|[0-9])*   return IDENTIFIER;
"/*"   { comment(); }

[ \t\v\f]*           ; 
"\n"      ++linenum;

.                             { yyerror("Unknown token"); exit(1); }
%%

void comment(void) {
 char c, prev = 0;
  
 while ((c = input()) != 0) {
  if (c == '/' && prev == '*')
   return;
  prev = c;
 }
}