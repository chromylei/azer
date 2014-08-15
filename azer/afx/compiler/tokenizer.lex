%option noyywrap
%x C_COMMENT
%{
#include <iostream>
#include "azer/afx/compiler/parser.h"
#include "azer/afx/compiler/debug.h"
#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/loc.h"
#include "base/strings/string_number_conversions.h"
#include "base/logging.h"
#include "afxlang_tab.h"

#define TYPE_INIT(typeconst, ttype)                \
  LEX_TRACE << "type: " << yytext << std::endl;    \
  yylval->spec.loc.lineno = context->lineno();     \
  yylval->spec.type = azer::afx::##ttype;     \
  return (typeconst);

#define KEYWORD_INIT(typeconst)                       \
  LEX_TRACE << "keyword: " << yytext << std::endl;    \
  yylval->spec.loc.lineno = context->lineno();        \
  return (typeconst);

#define PUNCTUATION_INIT(PUNCT)                         \
  LEX_TRACE << "punctuation: " << yytext << std::endl;  \
  yylval->lex.loc.lineno = context->lineno();           \
  return PUNCT;
  
%}

%%

"void"  {TYPE_INIT(VOID, kVoid);}
"bool"  {TYPE_INIT(BOOL, kBoolean);}
"char"  {TYPE_INIT(CHAR, kChar);}
"short" {TYPE_INIT(SHORT, kShort);}
"int"   {TYPE_INIT(INT, kInt32);}
"uint"  {TYPE_INIT(UINT, kUint32);}
"float" {TYPE_INIT(FLOAT, kFloat);}
"ivec2"  {TYPE_INIT(VEC2, kIntVec2);}
"ivec3"  {TYPE_INIT(VEC3, kIntVec3);}
"ivec4"  {TYPE_INIT(VEC4, kIntVec4);}
"vec2"  {TYPE_INIT(VEC2, kVector2);}
"vec3"  {TYPE_INIT(VEC3, kVector3);}
"vec4"  {TYPE_INIT(VEC4, kVector4);}
"mat3"  {TYPE_INIT(MAT3, kMatrix3);}
"mat4"  {TYPE_INIT(MAT4, kMatrix4);}
"Texture1D"  {TYPE_INIT(MAT4, kTexture1D);}
"Texture1DArray"  {TYPE_INIT(MAT4, kTexture1DArray);}
"Texture2D"  {TYPE_INIT(MAT4, kTexture2D);}
"Texture2DArray"  {TYPE_INIT(MAT4, kTexture2DArray);}
"Texture3D"  {TYPE_INIT(MAT4, kTexture3D);}
"TextureCube"  {TYPE_INIT(MAT4, kTextureCube);}

"if"        {KEYWORD_INIT(IF);}
"else"      {KEYWORD_INIT(ELSE);}
"do"        {KEYWORD_INIT(DO);}
"while"     {KEYWORD_INIT(WHILE);}
"for"       {KEYWORD_INIT(FOR);}
"switch"    {KEYWORD_INIT(SWITCH);}
"case"      {KEYWORD_INIT(CASE);}
"default"   {KEYWORD_INIT(DEFAULT);}
"continue"  {KEYWORD_INIT(CONTINUE);}
"break"     {KEYWORD_INIT(BREAK);}
"return"    {KEYWORD_INIT(RETURN);}
"discard"   {KEYWORD_INIT(DISCARD);}
"struct"    {KEYWORD_INIT(STRUCT);}

"technique" {KEYWORD_INIT(TECHNIQUE);}
"exchange"   {KEYWORD_INIT(EXCHANGE);}
"package"   {KEYWORD_INIT(PACKAGE);}

"in"      {
  LEX_TRACE << "qualifier: " << yytext<< std::endl;
  yylval->spec.storage_qualifier = azer::afx::kIn;
  return   STORAGE_QUALIFIER;
}
"out"     {
  LEX_TRACE << "qualifier: " << yytext<< std::endl;
  yylval->spec.storage_qualifier = azer::afx::kOut;
  return STORAGE_QUALIFIER;
}
"inout"   {
  LEX_TRACE << "qualifier: " << yytext<< std::endl;
  yylval->spec.storage_qualifier = azer::afx::kInout;
  return STORAGE_QUALIFIER;
}
"const"   {
  LEX_TRACE << "qualifier: " << yytext<< std::endl;
  yylval->spec.storage_qualifier = azer::afx::kConst;
  return STORAGE_QUALIFIER;
}
"uniform" {
  LEX_TRACE << "qualifier: " << yytext << std::endl;
  yylval->spec.loc.lineno = context->lineno();
  yylval->spec.storage_qualifier = azer::afx::kUniform;
  return STORAGE_QUALIFIER;
}

"true" {
  LEX_TRACE << "true: " << yytext << std::endl;
  yylval->lex.loc.lineno = context->lineno();
  yylval->lex.b = true;
  return BOOLCONST;
}

"false" {
  LEX_TRACE << "false: " << yytext << std::endl;
  yylval->lex.loc.lineno = context->lineno();
  yylval->lex.b = false;
  return BOOLCONST;
}

"++"  { PUNCTUATION_INIT(INC_OP);}
"--"  { PUNCTUATION_INIT(DEC_OP);}

"&&" { PUNCTUATION_INIT(LAND_OP);}
"||" { PUNCTUATION_INIT(LOR_OP);}
"^^" { PUNCTUATION_INIT(LXOR_OP);}
">=" { PUNCTUATION_INIT(GE_OP);}

"<=" { PUNCTUATION_INIT(LE_OP);}
"!=" { PUNCTUATION_INIT(NE_OP);}
"==" { PUNCTUATION_INIT(EQ_OP);}
"<<" { PUNCTUATION_INIT(LSHIFT_OP);}
">>" { PUNCTUATION_INIT(RSHIFT_OP);}

"=" { PUNCTUATION_INIT(EQUAL);}
"+=" { PUNCTUATION_INIT(ADD_ASSIGN);}
"-=" { PUNCTUATION_INIT(SUB_ASSIGN);}
"*=" { PUNCTUATION_INIT(MUL_ASSIGN);}
"/=" { PUNCTUATION_INIT(DIV_ASSIGN);}
"%=" { PUNCTUATION_INIT(MOD_ASSIGN);}
"&=" { PUNCTUATION_INIT(AND_ASSIGN);}
"|=" { PUNCTUATION_INIT(OR_ASSIGN);}
"^=" { PUNCTUATION_INIT(XOR_ASSIGN);}
">>=" { PUNCTUATION_INIT(RSHIFT_ASSIGN);}
"<<=" { PUNCTUATION_INIT(LSHIFT_ASSIGN);}

"(" { PUNCTUATION_INIT(LEFT_PAREN);}
")" { PUNCTUATION_INIT(RIGHT_PAREN);}
"[" { PUNCTUATION_INIT(LEFT_BRACKET);}
"]" { PUNCTUATION_INIT(RIGHT_BRACKET);}

"!" { PUNCTUATION_INIT(BANG);}
"+"   { PUNCTUATION_INIT(PLUS);}
"-"   { PUNCTUATION_INIT(DASH);}
"*"   { PUNCTUATION_INIT(START);}
"/"   { PUNCTUATION_INIT(SLASH);}
"%"   { PUNCTUATION_INIT(PERCENT);}
"&"   { PUNCTUATION_INIT(AMPERSAND);}
"?"   { PUNCTUATION_INIT(QUESTION);}
"|"  { PUNCTUATION_INIT(VERTICAL);}
"^"  { PUNCTUATION_INIT(CARET);}
"<"  { PUNCTUATION_INIT(LEFT_ANGLE);}
">"  { PUNCTUATION_INIT(RIGHT_ANGLE);}
";"  { PUNCTUATION_INIT(SEMICOLON);}
":"  { PUNCTUATION_INIT(COLON);}
","  { PUNCTUATION_INIT(COMMA);}
"."  { PUNCTUATION_INIT(DOT);}
"{"  { PUNCTUATION_INIT(LEFT_BRACE);}
"}"  { PUNCTUATION_INIT(RIGHT_BRACE);}
"#"  { PUNCTUATION_INIT(SHARP);}

[0-9]+\.[0-9]+[f]?         {
  LEX_TRACE << "double: " << yytext << std::endl;
  int length = yyleng;
  if (yytext[yyleng-1] == 'f') --length;
  std::string tex(yytext, length);
  yylval->lex.loc.lineno = context->lineno();
  CHECK(::base::StringToDouble(tex, &yylval->lex.d)) << yytext;
  return FLOATCONST;
}

[0-9]?\.[0-9]+[f]?         {
  LEX_TRACE << "double: " << yytext << std::endl;
  int length = yyleng;
  if (yytext[yyleng-1] == 'f') --length;
  std::string tex(yytext, length);
  yylval->lex.loc.lineno = context->lineno();
  CHECK(::base::StringToDouble(tex, &yylval->lex.d)) << yytext;
  return FLOATCONST;
}

[0-9]+                     {
  LEX_TRACE << "integer: " << yytext << std::endl;
  yylval->lex.loc.lineno = context->lineno();
  CHECK(::base::StringToInt(yytext, &yylval->lex.i));
  return INTCONST;
}

[0-9]+u                     {
  LEX_TRACE << "uint: " << yytext << std::endl;
  std::string tex(yytext, yyleng - 1);
  yylval->lex.loc.lineno = context->lineno();
  CHECK (::base::StringToUint(tex, &yylval->lex.u));
  return UINTCONST;
}

0x[0-9]+                   {
  LEX_TRACE << "hex: " << yytext << std::endl;
  CHECK (::base::HexStringToInt(yytext, &yylval->lex.i));
  yylval->lex.loc.lineno = context->lineno();
  return INTCONST;
}

\"([^"\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+)*\" {
  yylval->lex.string = new std::string(yytext, yyleng);
  return STRING;
}
[ \t\r]+ {LEX_TRACE << "space" << std::endl;}

"\0"                       {
  LEX_TRACE << "EOF" << std::endl;
  return KEOF;
 }
"\n" {
  LEX_TRACE << "newline" << std::endl;
  context->lineno_plus();
 }

[a-zA-Z_][a-zA-Z0-9_]*\:\: {
  LEX_TRACE << "package: " << yytext << std::endl;
  using azer::afx::ParseContext;
  context->AppendExternal(yytext);
}

[a-zA-Z_][a-zA-Z0-9_]* {
  LEX_TRACE << "identifier: " << yytext << std::endl;
  yylval->lex.loc.lineno = context->lineno();
  yylval->lex.identifier = new std::string(yytext, yyleng);
  using azer::afx::ParseContext;

  ParseContext::SymbolType stype =
      context->LookupSymbolType(*yylval->lex.identifier);
  
  bool external = !context->GetExternPrefix().empty();
  if (external) {
    yylval->lex.identifier->insert(0, context->GetExternPrefix() + "::");
    context->ClearExternSymbol(); 
  }

  if (stype == ParseContext::kStructureDecl) {
    // append current package name
    if (!context->package().empty()) {
      std::string prefix = context->package();
      prefix.append("::");
      yylval->lex.identifier->insert(0, prefix);
    }
    return STRUCT_TYPE;
  } else if (stype == ParseContext::kExternStructDecl) {
    return STRUCT_TYPE;
  } else {
    if (external && stype != ParseContext::kExternFuncSymbol) {
      std::stringstream ss;
      ss << "cannot find external symbol: \""
         << context->GetExternPrefix()
         << "::" << yytext << "\"";
      context->ReportError(yylval->lex.loc, ss.str());
      return NOMATCH;
    } else {
      return IDENTIFIER;
    }
  }
}

\'[^\']\' {
  LEX_TRACE << "char: " << yytext << std::endl;
  yylval->lex.loc.lineno = context->lineno();
  yylval->lex.c = *(yytext + 1);
  return CHARCONST;
}

\/\/[^\n]* {
}

"/*"         { BEGIN(C_COMMENT);}
<C_COMMENT>"*/" { BEGIN(INITIAL);}
<C_COMMENT>.  {}

. { return NOMATCH;}
%%
