%{
#include <iostream>
#include "base/basictypes.h"
#include "azer/afx/compiler/loc.h"
#include "azer/afx/compiler/debug.h"
#include "azer/afx/preprocessor/parser.h"
#include "base/strings/utf_string_conversions.h"
#include "base/logging.h"

#ifdef _WIN32
#define YY_NO_UNISTD_H
#include <io.h>
#endif

using namespace azer::afx;
%}
%union {
  struct {
    ::azer::afx::SourceLoc loc;
    union {
      int32 i;
      uint32 u;
      bool b;
      double d;
      std::string* string;
      std::vector<std::string>* vec;
    };
  } lex;
};
   
%pure_parser  // enable thread safety
// %expect 1     // One shift reduce conflict because of if | else

%token<lex> PRE_CODE PRE_COMMENT PRE_PROCESSOR PRE_EOF PRE_NOMATCH;
%token<lex> PRE_BANG PRE_SHARP PRE_LEFT_PAREN PRE_RIGHT_PAREN PRE_ASSIGN;
%token<lex> PRE_OP_GE PRE_OP_LE PRE_OP_EQ PRE_OP_NE;
%token<lex> PRE_LEFT_ANGLE PRE_RIGHT_ANGLE PRE_STRING PRE_PACKAGE;
%token<lex> PRE_DEFINED PRE_DEFINE  PRE_INCLUDE PRE_UNDEF;
%token<lex> PRE_IF PRE_ENDIF PRE_ELSE PRE_ELIF PRE_IFDEF PRE_IFNDEF;

%type<lex> statement statement_list;
%type<lex> define_statement package_statement undef_statement;
%type<lex> define_proto define_parameters define_parameter_list define_param;
// %type<lex> full_if_statement if_statement else_statement elif_statement;
// %type<lex> _endif_ _elif_ _else_ _ifdef_ _ifndef_ _if_ ;
// %type<lex> defined_expression
%type<lex> _undef_ _package_ _define_ _include_;
%type<lex> code_string  include_statement;

%{
#define YY_DECL int yylex(YYSTYLE* yylval,                              \
                          azer::afx::PreprocessorParser* context)
#define YYPARSE_PARAM context
#define parseContext ((azer::afx::PreprocessorParser*)context)
#define YYLEX_PARAM parseContext
#define yyerror(msg) parseContext->ReportError(msg)

int yylex(YYSTYPE* yylval, azer::afx::PreprocessorParser* preprocessor) {
  return preprocessor->tokenizer(yylval);
}
%}

%%

preprocessor : statement_list PRE_EOF;

statement_list: statement {
  PRE_PARSER_TRACE << "statement_list" << std::endl;
 }
| statement_list statement {
  PRE_PARSER_TRACE << "statement_list" << std::endl;
 }
;

statement
: define_statement
| undef_statement
| package_statement
// | full_if_statement
| include_statement
| code_string {
  parseContext->AppendCode(*$1.string);
  delete $1.string;
 }
;

undef_statement: _undef_ PRE_STRING {
  PRE_PARSER_TRACE << "undef " << *$2.string << std::endl;
  parseContext->RemoveDefined(*$2.string);
  delete $2.string;
}
;

package_statement: _package_ PRE_STRING {
  PRE_PARSER_TRACE << "package " << *$2.string << std::endl;
  if (!parseContext->package().empty()) {
    parseContext->ReportError("package has been declared");
  }
  
  parseContext->SetPackageName(*$2.string);
  delete $2.string;
 }
;

include_statement: _include_  PRE_STRING {
  using ::base::FilePath;
  PRE_PARSER_TRACE << "include " << *$2.string << std::endl;
#ifdef OS_WIN
  parseContext->AddIncludes(FilePath(::base::UTF8ToWide(*$2.string)));
#else
  parseContext->AddIncludes(FilePath(*$2.string));
#endif
  delete $2.string;
 }
;

define_statement
: define_proto PRE_STRING {
  PRE_PARSER_TRACE << "define_statement" << std::endl;
  parseContext->SetDefinedValue(*$1.string, *$2.string);
  delete $1.string;
  delete $2.string;
 }
| define_proto {
  PRE_PARSER_TRACE << "define_statement" << std::endl;
 }
;

define_proto: _define_ PRE_STRING define_parameters {
  PRE_PARSER_TRACE << "define_proto" << std::endl;
  parseContext->SetDefinedParams(*$2.string, $3.vec);
  $$ = $2;
  delete $3.vec;
 }
| _define_ PRE_STRING {
  PRE_PARSER_TRACE << "define_proto" << std::endl;
  parseContext->AddDefined(*$2.string);
  $$ = $2;
 }
;

define_parameters:
PRE_LEFT_PAREN  PRE_RIGHT_PAREN {
  PRE_PARSER_TRACE << "define_parameters" << std::endl;
  $$.vec = new std::vector<std::string>;
 }
| PRE_LEFT_PAREN define_parameter_list PRE_RIGHT_PAREN {
  PRE_PARSER_TRACE << "define_parameters" << std::endl;
  $$ = $2;
}
;

define_parameter_list
: define_param {
  PRE_PARSER_TRACE << "define_parameter_list: define_param " << std::endl;
  $$.vec = new std::vector<std::string>;
  $$.vec->push_back(*$1.string);
  delete $1.string;
}
| define_parameter_list define_param {
  PRE_PARSER_TRACE << "define_parameter_list: define_parameter_list define_param"
               << std::endl;
  $$ = $1;
  $$.vec = new std::vector<std::string>;
  $$.vec->push_back(*$2.string);
  delete $2.string;
 }
;

define_param: PRE_STRING {
  PRE_PARSER_TRACE << "define_param" << std::endl;
  $$ = $1;
}
;

/*
full_if_statement : if_statement _endif_ {
}
| if_statement else_statement {
}
| if_statement elif_statement _endif_ {
}
;

if_statement : _if_ defined_expression statement_list {
  PRE_PARSER_TRACE << "#if" << std::endl;
}
| _ifdef_ PRE_STRING statement_list {
  PRE_PARSER_TRACE << "#ifdef" << std::endl;
}
| _ifndef_ PRE_STRING statement_list  {
  PRE_PARSER_TRACE << "#ifndef" << std::endl;
}
;

else_statement: _else_ statement_list _endif_ {
  }
;

elif_statement :_elif_ defined_expression statement_list {
}
;


defined_expression
: PRE_DEFINED PRE_LEFT_PAREN expression PRE_RIGHT_PAREN {
  PRE_PARSER_TRACE << "defined()" << std::endl;
}
| PRE_BANG PRE_DEFINED PRE_LEFT_PAREN expression PRE_RIGHT_PAREN {
  PRE_PARSER_TRACE << "!defined()" << std::endl;
}
;

expression: PRE_STRING {
  PRE_PARSER_TRACE << "expression()" << std::endl;
}
;

_endif_: PRE_SHARP PRE_ENDIF;
_elif_: PRE_SHARP PRE_ELIF;
_else_: PRE_SHARP PRE_ELSE;
_if_: PRE_SHARP PRE_IF;
_ifdef_: PRE_SHARP PRE_IFDEF;
_ifndef_: PRE_SHARP PRE_IFNDEF;
*/

_define_: PRE_SHARP PRE_DEFINE;
_package_: PRE_SHARP PRE_PACKAGE;
_include_: PRE_SHARP PRE_INCLUDE;
_undef_: PRE_SHARP PRE_UNDEF;

code_string
: PRE_CODE { $$ = $1;}
| PRE_COMMENT {$$ = $1;}
;
