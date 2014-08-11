%option noyywrap
%{
#include <iostream>
#include "azer/effect/compiler/parser.h"
#include "azer/effect/compiler/debug.h"
#include "azer/effect/compiler/type.h"
#include "azer/effect/compiler/loc.h"
#include "base/strings/string_number_conversions.h"
#include "base/logging.h"
#include "afxlang_tab.h"

#define TYPE_INIT(typeconst, ttype)                \
  LEX_TRACE << "type: " << yytext << std::endl;    \
  yylval->spec.loc.lineno = context->lineno();     \
  yylval->spec.type = azer::compiler::##ttype;     \
  return (typeconst);

#define KEYWORD_INIT(typeconst)                       \
  LEX_TRACE << "keyword: " << yytext << std::endl;    \
  yylval->spec.loc.lineno = context->lineno();        \
  return (typeconst);

#define PUNCTUATION_INIT(PUNCT)                 \
  yylval->lex.loc.lineno = context->lineno();   \
  return PUNCT;
  
%}

%%


"if"        {KEYWORD_INIT(IF);}
"else"      {KEYWORD_INIT(ELSE);}
"endif"     {KEYWORD_INIT(ELSE);}
"defined"   {KEYWORD_INIT(ELSE);}
"define"    {KEYWORD_INIT(ELSE);}
"include"   {KEYWORD_INIT(ELSE);}

"(" { PUNCTUATION_INIT(LEFT_PAREN);}
")" { PUNCTUATION_INIT(RIGHT_PAREN);}
"!" { PUNCTUATION_INIT(NOT);}
">" { PUNCTUATION_INIT(LEFT_ANGLE);}
"<" { PUNCTUATION_INIT(RIGHT_ANGLE);}
"#" { PUNCTUATION_INIT(SHARP);}

\"([^"\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+)*\" {
  yylval->lex.string = new std::string(yytext, yyleng);
  return STRING;
}



"\0"                       {
  LEX_TRACE << "EOF" << std::endl;
  return KEOF;
 }
"\n" {
  LEX_TRACE << "newline" << std::endl;
  context->lineno_plus();
  context->append("\n");
 }

\/\/[^\n]* {
}

"/*"         { BEGIN(C_COMMENT);}
<C_COMMENT>"*/" { BEGIN(INITIAL);}
<C_COMMENT>.  {}

. { return CODE;}
%%
