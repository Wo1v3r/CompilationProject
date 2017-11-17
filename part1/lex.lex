%%
"*"        return MULT;   //
"+"        return PLUS;   // 
"-"        return MINUS;  // 
"/"        return DIV;    // 
"^"        return POINTER;

"&&"      return AND;       //
"&"       return REFERENCE;
"||"      return OR;        //
"=="      return EQUALS;    //
">="      return GEQUALS;   //
"<="      return LEQUALS;   //
">"       return GREATER;   //
"<"       return LOWER;     //

"!="      return NEQUALS;   //
"!"       return NOT;       //?
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

"null"    return NULLVALUE;
[0-9]+    return NUM;

";"       return SEMICOLON; 
"("       return LEFTPAR;   //?
")"       return RIGHTPAR;  //?
"["       return LEFTBRAC;
"]"       return RIGHTBRAC; 
"{"       return LEFTCURL;
"}"       return RIGHTCURL;
"|"       return PIPE;
","       return COMMA;

[a-zA-Z_]([a-zA-Z_]|[0-9])*   return IDENTIFIER; //?
[ \t\v\n\f]*                  ; //?
%%