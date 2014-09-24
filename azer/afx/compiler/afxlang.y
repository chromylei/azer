
%{
#include <iostream>
#include "base/basictypes.h"
#include "azer/afx/compiler/loc.h"
#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/debug.h"
#include "azer/afx/compiler/parser.h"
#include "azer/afx/compiler/util.h"
#include "azer/afx/compiler/syntax_validator.h"
#include "base/logging.h"

#ifdef _WIN32
#define YY_NO_UNISTD_H
#include <io.h>
#endif

using namespace azer::afx;
extern char* yytext;

#define REGISTER_STRUCT(SNAME, RTYPE, LOC) {                            \
    ParseContext::SymbolType stype = parseContext->LookupSymbolType(SNAME); \
    azer::afx::CompileErrno err = azer::afx::kStructDeclared;              \
    if (stype != ParseContext::kUnknownSymbol) {                           \
      std::stringstream ss;                                             \
      ss << SNAME << " has been declared as " << SymbolTypeName(stype); \
      parseContext->ReportError(LOC, ss.str(), err);                    \
    } else {                                                            \
      parseContext->RegisteSymbolType(SNAME, RTYPE);                    \
    }                                                                   \
  }

#define REGISTER_FUNCDECL(SNAME, RTYPE, LOC) {                          \
    ParseContext::SymbolType stype = parseContext->LookupSymbolType(SNAME); \
    azer::afx::CompileErrno err = azer::afx::kFuncDefined;                 \
    if (stype == ParseContext::kFunctionSymbol) {                          \
    } else if (stype != ParseContext::kUnknownSymbol) {                       \
      std::stringstream ss;                                             \
      ss << SNAME << " has been declared as " << SymbolTypeName(stype); \
      parseContext->ReportError(LOC, ss.str(), err);                    \
    } else {                                                            \
      parseContext->RegisteSymbolType(SNAME, RTYPE);                    \
    }                                                                   \
  }

%}

%union {
  struct {
    azer::afx::SourceLoc loc;
    union {
      char c;
      int32 i;
      uint32 u;
      bool b;
      double d;
      std::string* string;
      std::string* identifier;
    };
  } lex;

  struct {
    azer::afx::SourceLoc loc;
    union {
      azer::afx::Operator op;
      azer::afx::BasicType type;
      azer::afx::StorageQualifier storage_qualifier;
    };
  } spec;

  struct {
    union {
      azer::afx::ASTNode*         node;
      azer::afx::BinaryOpNode*    binary_node;
      azer::afx::FieldNode*       fieldnode;
      azer::afx::AttributesNode*  attrnode;
      azer::afx::StructDeclNode*  struct_declare_node;
      azer::afx::FuncProtoNode*    func_declare_node;
      azer::afx::UnaryOpNode*     unary_node;
    };
  } astnode;
};


   
%pure_parser  // enable thread safety
%expect 1     // One shift reduce conflict because of if | else

%token<lex> IF ELSE DO WHILE FOR SWITCH CASE CONTINUE BREAK RETURN;
%token<lex> DISCARD DEFAULT STRUCT TECHNIQUE PACKAGE EXCHANGE STRUCT_TYPE;
%token<lex> CHARCONST INTCONST UINTCONST FLOATCONST BOOLCONST STRING IDENTIFIER;
%token<lex> EXTERN_PREFIX;
%token<spec> STORAGE_QUALIFIER;

%token<lex> NOMATCH;

// operators
%token<lex> DOT SEMICOLON COLON COMMA SHARP KEOF LEFT_PAREN RIGHT_PAREN;
%token<lex> LEFT_BRACKET RIGHT_BRACKET LEFT_BRACE RIGHT_BRACE;
%token<lex> PLUS DASH BANG TILDE START SLASH PERCENT LEFT_ANGLE RIGHT_ANGLE;
%token<lex> AMPERSAND VERTICAL CARET EQUAL QUESTION;

%token<lex> INC_OP DEC_OP LAND_OP LOR_OP LXOR_OP;
%token<lex> GE_OP LE_OP NE_OP EQ_OP;
%token<lex> LSHIFT_OP RSHIFT_OP;
%token<lex> ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN;
%token<lex> AND_ASSIGN OR_ASSIGN XOR_ASSIGN RSHIFT_ASSIGN LSHIFT_ASSIGN;

// basic type
%token<spec> VOID BOOL CHAR SHORT INT UINT FLOAT;
%token<spec> VEC2 VEC3 VEC4 MAT2 MAT3 MAT4;
%token<spec> SAMPLER1D SAMPLER2D SAMPLER3D SAMPLERCUBE;

%type<astnode.node> fully_specifier_type;
%type<astnode.node> type_specifier array_specifier type_specifier_nonarray;
%type<spec> type_qualifier single_type_qualifier;
// structure
%type<astnode.fieldnode> struct_declarator struct_declarator_list;
%type<astnode.fieldnode> struct_declaration_list struct_declaration;
%type<astnode.node> struct_specifier anonymous_specifier;

// function
%type<astnode.node> parameter_declaration parameter_declarator;
%type<astnode.node> parameter_type_specifier;
%type<astnode.func_declare_node> function_header function_header_with_parameters;
%type<astnode.func_declare_node> function_prototype;


%type<astnode.attrnode> attributes attribute_list;
%type<astnode.node> single_declaration init_declarator_list;
%type<astnode.node> declaration;
%type<astnode.node> initializer initializer_list;
// %type<lex> identifier_list;

// expression
%type<spec> unary_operator assignment_operator
%type<astnode.node> variable_identifier primary_expression postfix_expression;
%type<astnode.node> unary_expression;
%type<astnode.node> multiplicative_expression additive_expression;
%type<astnode.node> shift_expression relational_expression equality_expression;
%type<astnode.node> and_expression exclusive_or_expression;
%type<astnode.node> inclusive_or_expression logical_and_expression;
%type<astnode.node> logical_xor_expression logical_or_expression;
%type<astnode.node> conditional_expression assignment_expression;
%type<astnode.node> expression integer_expression constant_expression;

// statement
%type<astnode.node> expression_statement declaration_statement
%type<astnode.node> attributed_declaration_statement;
%type<astnode.node> attributed_function_definition;
%type<astnode.node> statement_list statement simple_statement;
%type<astnode.node> compound_statement compound_statement_no_new_scope;

// flow control
%type<astnode.node> switch_statement case_label switch_statement_list;
%type<astnode.node> jump_statement while_statement do_while_statement;
%type<astnode.node> statement_no_new_scope statement_scoped;
%type<astnode.node> for_statement for_init_statement for_condition_statement
for_reset_statement;
%type<astnode.node> selection_statement selection_reset_statement;

%type<astnode.node> function_definition;
%type<astnode.node> function_call_generic;
%type<astnode.node> function_call_header_no_parameters;
%type<astnode.node> function_call_header_with_parameters;
%type<astnode.node> function_call_header function_identifier function_call;
%type<astnode.node> external_declaration technique_declaration;
%start translation_unit

%{
#define YYPARSE_PARAM context
#define parseContext ((azer::afx::ParseContext*)context)
#define YYLEX_PARAM parseContext
#define yyerror(msg) parseContext->ReportError(msg, yytext)

extern int yylex(YYSTYPE* yylval, azer::afx::ParseContext*);
%}
%%

// Grammar Note:  No 'goto'.  Gotos are not supported.

translation_unit
: external_declaration {
  PARSER_TRACE << "translation_unit";
 }
| translation_unit external_declaration {
  PARSER_TRACE << "translation_unit";
 }
;

external_declaration
: function_definition {
  PARSER_TRACE << "external_declaration: function_definition\n";
  parseContext->AddFunction($1);
 }
| attributed_function_definition {
  PARSER_TRACE << "external_declaration: attributed_function_definition\n";
  parseContext->AddFunction($1);
  }
| declaration {
  PARSER_TRACE << "external_declaration: declaration" << std::endl;
  if ($1->IsDeclarationNode()) {
    parseContext->AddVariableDecl($1);
  } else if ($1->IsStructDeclNode()) {
  } else {
  }
}
| attributed_declaration_statement {
  PARSER_TRACE << "external_declaration: attributed_declaration_statement\n";
  if ($1->IsDeclarationNode()) {
    parseContext->AddVariableDecl($1);
  }
}
| technique_declaration {
  PARSER_TRACE << "external_declaration: technique_declaration\n";
  parseContext->AddTechnique($1);
  }
;

statement_list : statement {
  PARSER_TRACE << "statement_list: statement" << std::endl;
  $$ = $1;
 }
| statement_list statement {
  PARSER_TRACE << "statement_list: statement_list statement" << std::endl;
  $$ = $1;
  if ($2 != NULL) $$->AddSibling($2);
 }
;

statement
: simple_statement {
  PARSER_TRACE << "statement: simple_statement" << std::endl;
  $$ = CreateStatement($1, parseContext);
 }
| compound_statement {
  PARSER_TRACE << "statement: compound_statement" << std::endl;
  $$ = CreateStatement($1, parseContext);
  }
;

simple_statement
: declaration_statement {$$ = $1;}
| expression_statement {$$ = $1;}
| selection_statement {$$ = $1;}
| switch_statement {$$ = $1;}
| case_label {$$ = $1;}
| while_statement {$$ = $1;}
| do_while_statement {$$ = $1;}
| for_statement {$$ = $1;}
| jump_statement { $$ = $1;}
;


expression_statement
: SEMICOLON {
  PARSER_TRACE << "expression_statement: ;" << std::endl;
  $$ = 0;
 }
| expression SEMICOLON {
  PARSER_TRACE << "expression_statement: expression;" << std::endl;
  $$ = $1;
 }
;

declaration_statement
: declaration {
  PARSER_TRACE << "declaration_statement: declaration " << std::endl;
  $$ = $1;
 }

attributed_declaration_statement
: attributes declaration {
  DCHECK($1->IsAttributesNode());
  $2->SetAttributes($1);
  $$ = $2;
 }
;

compound_statement
: LEFT_BRACE RIGHT_BRACE {
  PARSER_TRACE << "coumpound_statement: { } " << std::endl;
  $$ = 0;
 }
| LEFT_BRACE statement_list RIGHT_BRACE {
  PARSER_TRACE << "coumpound_statement: {statement_list}" << std::endl;
  $$ = $2;
 }
;

statement_no_new_scope
: compound_statement_no_new_scope {
  PARSER_TRACE << "statement_no_new_scope" << std::endl;
  $$ = $1;
 }
| simple_statement                {
  PARSER_TRACE << "statement_no_new_scope" << std::endl;
  $$ = CreateStatement($1, parseContext);
  }
;

statement_scoped 
: compound_statement_no_new_scope {
  PARSER_TRACE << "statement_scoped" << std::endl;
  $$ = CreateScoped($1, parseContext);
}
| simple_statement                {
  PARSER_TRACE << "statement_scoped:" << std::endl;
  StatementNode* node = CreateStatement($1, parseContext);
  $$ = CreateScoped(node, parseContext);
}
;

compound_statement_no_new_scope
: LEFT_BRACE RIGHT_BRACE {
  PARSER_TRACE << "compound_statement_no_new_scope {}" << std::endl;
  $$ = parseContext->Create(ASTNode::kNullNode, $1.loc)->ToNullNode();
 }
| LEFT_BRACE statement_list RIGHT_BRACE {
  PARSER_TRACE << "compound_statement_no_new_scope {statement_list}" << std::endl;
  $$ = $2;
 }
;

technique_declaration: TECHNIQUE attributes SEMICOLON {
  $$ = $2;
 }
;

selection_statement
: IF LEFT_PAREN expression RIGHT_PAREN selection_reset_statement {
  $$ = $5;
  const SourceLoc& loc = $2.loc;
  IfElseNode* node  = $$->ToIfElseNode();
  node->SetSelectionExpr($3);
 }
;

selection_reset_statement
: statement_scoped ELSE statement_scoped {
  const SourceLoc& loc = $2.loc;
  IfElseNode* node  = parseContext->Create(ASTNode::kIfElseNode, loc)
      ->ToIfElseNode();
  node->SetTrueStatements($1);
  node->SetFalseStatements($3);
  $$ = node;
 }
| statement_scoped {
  PARSER_TRACE << "selection_reset_statement: statement_scoped" << std::endl;
  IfElseNode* node  = parseContext->Create(ASTNode::kIfElseNode, $1->loc())
      ->ToIfElseNode();
  node->SetTrueStatements($1);
  $$ = node;
  }
;


for_statement: FOR LEFT_PAREN for_init_statement for_condition_statement for_reset_statement RIGHT_PAREN statement_no_new_scope {
  PARSER_TRACE << "for_statement" << std::endl;
  ForLoopNode* node  = parseContext->Create(ASTNode::kForLoopNode, $1.loc)
      ->ToForLoopNode();
  node->SetScopedNode(parseContext->Create(ASTNode::kScopedNode, $1.loc)
                      ->ToScopedNode());
  node->SetInit($3);
  node->SetCondition($4);
  node->SetReset($5);
  node->SetStatements($7);
  $$ = node;
 };

for_init_statement: declaration_statement {
  PARSER_TRACE << "for_init_statement" << std::endl;
  $$ = $1;
 }
| expression_statement {
  PARSER_TRACE << "for_init_statement" << std::endl;
  $$ = $1;
  }
;

for_condition_statement: conditional_expression SEMICOLON {
  PARSER_TRACE << "for_condition_statement" << std::endl;
  $$ = $1;
 }
;

for_reset_statement: {$$ = 0; }
| expression {$$ = $1;}
;



while_statement : WHILE LEFT_PAREN expression RIGHT_PAREN statement_no_new_scope {
  PARSER_TRACE << "while_statement" << std::endl;
  WhileLoopNode* node  = (WhileLoopNode*)parseContext->Create(
      ASTNode::kWhileLoopNode, $2.loc);
  node->SetStatements($5);
  node->SetExprNode($3);
  $$ = node;
 }
;

do_while_statement : DO statement_no_new_scope WHILE LEFT_PAREN expression RIGHT_PAREN SEMICOLON {
  PARSER_TRACE << "do_while_statement" << std::endl;
  WhileLoopNode* node  = (WhileLoopNode*)parseContext->Create(
      ASTNode::kWhileLoopNode, $3.loc);
  node->SetStatements($2);
  node->SetExprNode($5);
  node->SetDoWhileStyle(true);
  $$ = node;
 }
;

jump_statement
: CONTINUE SEMICOLON {
  JumpNode* node =(JumpNode*)parseContext->Create(ASTNode::kJumpNode, $2.loc);
  node->SetJumpType(JumpNode::kContinue);
  $$ = node;
 }
| BREAK SEMICOLON {
  JumpNode* node =(JumpNode*)parseContext->Create(ASTNode::kJumpNode, $2.loc);
  node->SetJumpType(JumpNode::kBreak);
  $$ = node;
 }
| RETURN SEMICOLON {
  ReturnNode* node =(ReturnNode*)parseContext->Create(ASTNode::kReturnNode, $2.loc);
  $$ = node;
 }
| RETURN expression SEMICOLON {
  ReturnNode* node =(ReturnNode*)parseContext->Create(ASTNode::kReturnNode, $3.loc);
  node->SetExprNode($2);
  $$ = node;
 }
| DISCARD SEMICOLON {
  JumpNode* node =(JumpNode*)parseContext->Create(ASTNode::kJumpNode, $2.loc);
  node->SetJumpType(JumpNode::kDiscard);
  $$ = node;
}
;

// switch case
switch_statement
: SWITCH LEFT_PAREN expression RIGHT_PAREN LEFT_BRACE
       switch_statement_list RIGHT_BRACE {
  const SourceLoc& loc = $2.loc;
  SwitchNode* node =(SwitchNode*)parseContext->Create(ASTNode::kSwitchNode, loc);
  node->SetExpressionNode($3);
  node->SetStatements($6);
  $$ = node;
 }
;

switch_statement_list : { $$ = NULL; }
| statement_list { $$ = $1;}
;

case_label
: CASE expression COLON {
  CaseNode* node = (CaseNode*)parseContext->Create(ASTNode::kCaseNode, $3.loc);
  node->SetExpressionNode($2);
  $$ = node;
 }
| DEFAULT COLON {
  CaseNode* node = (CaseNode*)parseContext->Create(ASTNode::kCaseNode, $2.loc);
  node->SetAsDefaultNode();
  $$ = node;
 }
;

// variable declaration
declaration
: function_prototype SEMICOLON {
  PARSER_TRACE << "declaration: function_prototype; " << std::endl;
  $$ = $1;
  FuncProtoNode* node = $$->ToFuncProtoNode();
  REGISTER_FUNCDECL(node->funcname(), ParseContext::kFunctionSymbol, node->loc());
}
| init_declarator_list SEMICOLON {
  PARSER_TRACE << "declaration: init_declarator_list " << std::endl;
  $$ = $1;
  DCHECK($1->IsDeclarationNode());
 }
| struct_specifier SEMICOLON {
  PARSER_TRACE << "declaration: init_declarator_list ; " << std::endl;
  $$ = $1;
  DCHECK($1->IsStructDeclNode());
 }
| EXCHANGE struct_specifier SEMICOLON {
  PARSER_TRACE << "declaration: exchange struct_sepcifier ; " << std::endl;
  $$ = $2;
  DCHECK($2->IsStructDeclNode());
  $$->ToStructDeclNode()->SetExchangeStruct();
 }
;

init_declarator_list : single_declaration {
  PARSER_TRACE << "init_declaration_list" << std::endl;
  $$ = $1;
 }
| init_declarator_list COMMA IDENTIFIER {
  PARSER_TRACE << "init_declaration_list " << *$3.identifier << std::endl;
  DCHECK($1->IsDeclarationNode());
  DeclarationNode* decl = $1->ToDeclarationNode();
  TypedNode* typed = $1->ToDeclarationNode()->GetTypedNode();
  TypedNode* newtyped = CreateTypedNode(typed->GetType(), $2.loc, parseContext);
  CreateSymbolDecl(*$3.identifier, newtyped, decl, $2.loc, parseContext);
  $$ = $1;
  delete $3.identifier;
 }
| init_declarator_list COMMA IDENTIFIER array_specifier {
  PARSER_TRACE << "init_declaration_list " << *$3.identifier << std::endl;
  DCHECK($4->IsTypedNode());
  DCHECK($1->IsDeclarationNode());
  DeclarationNode* decl = $1->ToDeclarationNode();
  TypedNode* typed = $4->ToTypedNode();
  typed->SetType($1->ToDeclarationNode()->GetType());
  CreateSymbolDecl(*$3.identifier, typed, decl, $2.loc, parseContext);
  $$ = $1;
  delete $3.identifier;
 }
| init_declarator_list COMMA IDENTIFIER EQUAL initializer {
  PARSER_TRACE << "init_declaration_list " << *$3.identifier << std::endl;
  DCHECK($1->IsDeclarationNode());
  TypedNode* typed = $1->ToDeclarationNode()->GetTypedNode();
  TypedNode* newtyped = CreateTypedNode(typed->GetType(), $2.loc, parseContext);
  DeclarationNode* decl = $1->ToDeclarationNode();
  CreateSymbolDecl(*$3.identifier, newtyped, $5, decl, $3.loc, parseContext);
  $$ = $1;
  delete $3.identifier;
 }
| init_declarator_list COMMA IDENTIFIER array_specifier EQUAL initializer {
  PARSER_TRACE << "init_declaration_list " << *$3.identifier << std::endl;
  DCHECK($4->IsTypedNode());
  DCHECK($1->IsDeclarationNode());
  DeclarationNode* decl = $1->ToDeclarationNode();
  TypedNode* typed = $4->ToTypedNode();
  typed->SetType($1->ToDeclarationNode()->GetType());
  CreateSymbolDecl(*$3.identifier, typed, $6, decl, $3.loc, parseContext);
  $$ = $1;
  delete $3.identifier;
 }
;

single_declaration
: fully_specifier_type IDENTIFIER {
  PARSER_TRACE << "single declaration: " << *$2.identifier << std::endl;
  DCHECK($1->IsTypedNode());
  TypedNode* typednode = $1->ToTypedNode();
  $$  = CreateDeclSymbolNode(*$2.identifier, typednode, $2.loc, parseContext);
  delete $2.identifier;
 }
| fully_specifier_type IDENTIFIER array_specifier {
  PARSER_TRACE << "single declaration []: " << *$2.identifier << std::endl;
  DCHECK($1->IsTypedNode());
  DCHECK($3->IsTypedNode());
  TypedNode* typednode = $3->ToTypedNode();
  typednode->SetType($1->ToTypedNode()->GetType());
  $$ = CreateDeclSymbolNode(*$2.identifier, typednode, $2.loc, parseContext);
  delete $2.identifier;
 }
| fully_specifier_type IDENTIFIER EQUAL initializer {
  PARSER_TRACE << "single declaration: " << *$2.identifier << std::endl;
  DCHECK($1->IsTypedNode());
  TypedNode* typednode = $1->ToTypedNode();
  $$ = CreateDeclSymbolNode(*$2.identifier, typednode, $4, $2.loc, parseContext);
  delete $2.identifier;
 }
| fully_specifier_type IDENTIFIER array_specifier EQUAL initializer {
  PARSER_TRACE << "single declaration []: " << *$2.identifier << std::endl;
  DCHECK($1->IsTypedNode());
  DCHECK($3->IsTypedNode());
  TypedNode* typednode = $3->ToTypedNode();
  typednode->SetType($1->ToTypedNode()->GetType());
  $$ = CreateDeclSymbolNode(*$2.identifier, typednode, $5, $2.loc, parseContext);
  delete $2.identifier;
 }
;

/**
 * function_call
 */
function_call
: function_call_generic {
  PARSER_TRACE << "function_call " << std::endl;
  $$ = $1;
  FuncCallNode* node = $$->ToFuncCallNode();
 }
;

function_call_generic
: function_call_header_with_parameters RIGHT_PAREN {
  PARSER_TRACE << "function_call_generic " << std::endl;
  $$ = $1;
 }
| function_call_header_no_parameters RIGHT_PAREN {
  PARSER_TRACE << "function_call_generic " << std::endl;
  $$ = $1;
 }
;

function_call_header_with_parameters
: function_call_header assignment_expression {
  PARSER_TRACE << "function_call_header_with_parameters " << std::endl;
  $$ = $1;
  DCHECK($$->IsFuncCallNode());
  $$->ToFuncCallNode()->AddParam($2);
}
| function_call_header_with_parameters COMMA assignment_expression {
  PARSER_TRACE << "function_call_header_with_parameters " << std::endl;
  $$ = $1;
  DCHECK($$->IsFuncCallNode());
  $$->ToFuncCallNode()->AddParam($3);
}
;

function_call_header_no_parameters
: function_call_header VOID {
  PARSER_TRACE << "function_call_header_no_parameters " << std::endl;
  $$ = $1;
 }
| function_call_header {
  PARSER_TRACE << "function_call_header_no_parameters " << std::endl;
  $$ = $1;
  }
;

function_call_header
: function_identifier LEFT_PAREN {
  PARSER_TRACE << "function_call_header " << std::endl;
  $$ = $1;
 }
;

function_identifier
: type_specifier {
  PARSER_TRACE << "function_identifier: type_specifier " << std::endl;
  DCHECK($1->IsTypedNode());
  const SourceLoc& loc = $1->loc();
  FuncCallTypeInitNode* func = parseContext->Create(ASTNode::kFuncCallTypeInitNode,
                                                    loc)->ToFuncCallTypeInitNode();
  TypePtr typeptr($1->ToTypedNode()->GetType());
  if (typeptr->IsBasicType()) {
    func->SetType(typeptr);
  } else {
    std::stringstream ss;
    ss << " unkonwn type func \"" <<typeptr->name() << "\"";
    parseContext->ReportError(loc, ss.str());
  }
  $$ = func;
}
| postfix_expression {
  const SourceLoc& loc = $1->loc();
  PARSER_TRACE << "function_identifier: postfix_expression " << std::endl;
  DCHECK($1->IsRefSymbolNode());
  RefSymbolNode* symbol = $1->ToRefSymbolNode();
  FuncCallNode* func = parseContext->Create(ASTNode::kFuncCallNode,
                                            loc)->ToFuncCallNode();
  func->SetFuncName(symbol->symbolname());
  $$ = func;
}
;

/**
 * function declaration
 */
function_prototype
: function_header  RIGHT_PAREN {
  PARSER_TRACE << "function_prototype" << std::endl;
  $$ = $1;
}
| function_header_with_parameters RIGHT_PAREN {
  PARSER_TRACE << "function_prototype" << std::endl;
  $$ = $1;
  }
;

function_header_with_parameters
: function_header parameter_declaration {
  PARSER_TRACE << "function_header_with_parameters" << std::endl;
  $$ = $1;
  $$->AddParam($2);
}
| function_header_with_parameters COMMA parameter_declaration {
  PARSER_TRACE << "function_header_with_parameters" << std::endl;
  $$ = $1;
  $$->AddParam($3);
}
;

function_header
: fully_specifier_type IDENTIFIER LEFT_PAREN {
  PARSER_TRACE << "function_header" << std::endl;
  const SourceLoc& loc = $2.loc;
  DCHECK($1->IsTypedNode());
  $$ = CreateFuncProto(*$2.identifier, $1->ToTypedNode(), loc, parseContext);
  delete $2.identifier;
}
;

attributed_function_definition
: attributes function_definition {
  PARSER_TRACE << "attributed_function_definition" << std::endl;
  $$ = $2;
  DCHECK($1->IsAttributesNode());
  DCHECK($2->IsFuncDefNode());
  $$->ToFuncDefNode()->SetAttributes($1);
 }
;


function_definition
: function_prototype compound_statement_no_new_scope {
  DCHECK($1->IsFuncProtoNode());
  const SourceLoc& loc = $1->loc();
  FuncDefNode* func = parseContext->Create(ASTNode::kFuncDefNode, loc)
      ->ToFuncDefNode();
  func->SetStatements(CreateScoped($2, parseContext));
  func->SetProtoNode($1);
  $$ = func;
  PARSER_TRACE << "function_definition: "<<  func->funcname() << std::endl;
  REGISTER_FUNCDECL(func->funcname(), ParseContext::kFunctionSymbol, func->loc());
 }
;

parameter_declaration
: type_qualifier parameter_declarator {
  ParamNode* param = $2->ToParamNode();
  param->GetType()->SetStorageQualifier($1.storage_qualifier);
  $$ = param;
}
| parameter_declarator {
  $$ = $1;
  }
/// parameter without name
| type_qualifier parameter_type_specifier {
  ParamNode* param = $2->ToParamNode();
  param->GetType()->SetStorageQualifier($1.storage_qualifier);
  $$ = param;
}
| parameter_type_specifier {
  $$ = $1;
  }
;

parameter_declarator
: type_specifier IDENTIFIER {
  PARSER_TRACE << "parameter_declarator " << std::endl;
  const SourceLoc& loc = $1->loc();
  DCHECK($1->IsTypedNode());
  TypedNode* typed = $1->ToTypedNode();
  ParamNode* param = CreateParamNode(*$2.identifier, typed, loc, parseContext);

  $$ = param;
  delete $2.identifier;
}
| type_specifier IDENTIFIER array_specifier {
  PARSER_TRACE << "parameter_declarator  []" << std::endl;
  const SourceLoc& loc = $2.loc;
  DCHECK($1->IsTypedNode());
  DCHECK($3->IsTypedNode());
  TypedNode* typednode = $3->ToTypedNode();
  typednode->SetType($1->ToTypedNode()->GetType());
  ParamNode* param = CreateParamNode(*$2.identifier, typednode, loc, parseContext);
  $$ = param;
  delete $2.identifier;
}
;
  

parameter_type_specifier
: type_specifier {
  PARSER_TRACE << "parameter_type_specifier" << std::endl;
  DCHECK($1->IsTypedNode());
  $$ = parseContext->Create(ASTNode::kParamNode, $1->loc())->ToParamNode();
  $$->ToParamNode()->SetTypedNode($1);
}
;

/**
 * structure declaration
 */
struct_specifier
: STRUCT IDENTIFIER LEFT_BRACE struct_declaration_list RIGHT_BRACE {
  PARSER_TRACE << "struct_specifier " << std::endl;
  const SourceLoc& loc = $2.loc;
  $$ = (StructDeclNode*)parseContext->Create(ASTNode::kStructDeclNode, loc);
  StructDeclNode* node = $$->ToStructDeclNode();
  node->SetStructName(*$2.identifier);
  node->AddFields($4);
  delete $2.identifier;

  REGISTER_STRUCT(node->struct_name(), ParseContext::kStructureDecl, node->loc());
  parseContext->AddStructDecl(node);
 }
;

anonymous_specifier: STRUCT LEFT_BRACE struct_declaration_list RIGHT_BRACE {
  PARSER_TRACE << "anonymous_specifier " << std::endl;
  const SourceLoc& loc = $2.loc;
  $$ = (StructDeclNode*)parseContext->Create(ASTNode::kStructDeclNode, loc);
  StructDeclNode* node = $$->ToStructDeclNode();
  node->AddFields($3);

  REGISTER_STRUCT(node->struct_name(), ParseContext::kStructureDecl, node->loc());
 }
;

// int a, b[], c;
// float d, e, f;
struct_declaration_list
: struct_declaration {
  PARSER_TRACE << "struct_declaration_list " << std::endl;
  $$ = $1;
 }
| struct_declaration_list struct_declaration {
  PARSER_TRACE << "struct_declaration_list append ["
               << $2->fieldname() << "]" << std::endl;
  $$ = $1;
  $$->AddSibling($2);
 }
;

// int a, b[], c;
struct_declaration
: type_specifier struct_declarator_list SEMICOLON {
  PARSER_TRACE << "struct_declaration " << std::endl;
  $$ = $2;
  DCHECK($1->IsTypedNode());
  TypedNode* typednode = $1->ToTypedNode();
  ASTNode* cur = $2;
  while (cur) {
    DCHECK(cur->IsFieldNode());
    FieldNode* node = cur->ToFieldNode();
    if (node->GetTypedNode()) {
      TypedNode* raw = node->GetTypedNode();
      raw->SetType(typednode->GetType());
    } else {
      node->SetTypedNode(typednode);
    }
    cur = cur->next_sibling();
  }
 }
| attributes type_specifier struct_declarator_list SEMICOLON {
  PARSER_TRACE << "struct_declaration " << std::endl;
  $$ = $3;
  DCHECK($2->IsTypedNode());
  TypedNode* typednode = $2->ToTypedNode();
  ASTNode* cur = $3;
  while (cur) {
    DCHECK(cur->IsFieldNode());
    FieldNode* node = cur->ToFieldNode();
    if (node->GetTypedNode()) {
      TypedNode* raw = node->GetTypedNode();
      raw->SetType(typednode->GetType());
    } else {
      node->SetTypedNode(typednode);
    }
    node->SetAttributes($1);
    cur = cur->next_sibling();
  }
 }
;

// a, b[], c
struct_declarator_list
: struct_declarator {
  PARSER_TRACE << "struct_declarator_list " << std::endl;
  $$ = $1;
 }
| struct_declarator_list COMMA struct_declarator {
  PARSER_TRACE << "struct_declarator_list " << std::endl;
  $$->AddSibling($3);
 }
;

struct_declarator : IDENTIFIER {
  PARSER_TRACE << "struct_declarator: " << *$1.identifier << std::endl;
  const SourceLoc& loc = $1.loc;
  FieldNode* field = (FieldNode*)parseContext->Create(ASTNode::kFieldNode, loc);
  field->SetFieldName(*$1.identifier);
  $$ = field;
  delete $1.identifier;
 }
| IDENTIFIER array_specifier {
  PARSER_TRACE << "struct_declarator [] "  << std::endl;
  const SourceLoc& loc = $1.loc;
  DCHECK($2->IsTypedNode());
  $$ = CreateFieldNode(*$1.identifier, $2->ToTypedNode(), loc, parseContext);
  delete $1.identifier;
 }
;

// initialize

fully_specifier_type
: type_specifier {
  PARSER_TRACE << "fully_specifier_type " << std::endl;
  DCHECK($1->IsTypedNode());
  $$ = $1;
 }
| type_qualifier type_specifier {
  PARSER_TRACE << "fully_specifier_type with qualifier " << std::endl;
  DCHECK($2->IsTypedNode());
  $$ = $2;
  $$->ToTypedNode()->GetType()->SetStorageQualifier($1.storage_qualifier);
 }
;

type_specifier
: type_specifier_nonarray {
  PARSER_TRACE << "type_specifier " << std::endl;
  $$ = $1;
 }
| type_specifier_nonarray array_specifier {
  PARSER_TRACE << "type_specifier with array_specifier " << std::endl;
  // 删除之前创建的 type, 转而使用新创建的
  DCHECK($1->IsTypedNode());
  DCHECK($2->IsTypedNode());
  $2->ToTypedNode()->SetType($1->ToTypedNode()->GetType());
  $$ = $2;
 }
;

array_specifier
: LEFT_BRACKET RIGHT_BRACKET {
  PARSER_TRACE << "array_specifier[] " << std::endl;
  $$ = CreateArraySpecifierTypedNode($1.loc, parseContext);
}
| LEFT_BRACKET constant_expression RIGHT_BRACKET {
  PARSER_TRACE << "array_specifier[const] " << std::endl;
  $$ = CreateArraySpecifierTypedNode($2, $1.loc, parseContext);
 }
| array_specifier LEFT_BRACKET RIGHT_BRACKET {
  PARSER_TRACE << "array_specifier[][] " << std::endl;
  DCHECK($$->IsTypedNode());
  $$->ToTypedNode()->AddDim(NULL);
  }
| array_specifier LEFT_BRACKET constant_expression RIGHT_BRACKET {
  PARSER_TRACE << "array_specifier[][const] " << std::endl;
  DCHECK($$->IsTypedNode());
  $$->ToTypedNode()->AddDim($3);
 }
;

// qualifier
type_qualifier : single_type_qualifier {
  PARSER_TRACE << "type_qualifier " << std::endl;
  $$ = $1;
 }
| type_qualifier single_type_qualifier {
  PARSER_TRACE << "type_qualifier " << std::endl;
  $$ = $1;
  $$.storage_qualifier = (StorageQualifier)
      ($$.storage_qualifier | $2.storage_qualifier);
 }
;

single_type_qualifier : STORAGE_QUALIFIER {
  $$ = $1;
 }
;

type_specifier_nonarray
: VOID {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
 }
| BOOL {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| CHAR {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| SHORT {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| INT {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| UINT {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| FLOAT {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| VEC2 {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| VEC3 {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| VEC4 {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| MAT2 {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| MAT3 {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| MAT4 {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| SAMPLER1D {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| SAMPLER2D {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| SAMPLER3D {
  PARSER_TRACE << "type_specifier_nonarray " << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| SAMPLERCUBE {
  PARSER_TRACE << "type_specifier_nonarray SamplerCube" << std::endl;
  $$ = CreateTypedNode($1.type, $1.loc, parseContext);
  }
| struct_specifier {
  DCHECK($1->IsStructDeclNode());
  StructDeclNode* node = $1->ToStructDeclNode();
  PARSER_TRACE << "type_specifier_nonarray Struct " << node->struct_name()
               << std::endl;
  $$ = CreateTypedNode(node->struct_name(), $1->loc(), parseContext);
  }
| STRUCT_TYPE {
  PARSER_TRACE << "type_specifier_nonarray Sturct "<< *$1.identifier << std::endl;
  $$ = CreateTypedNode(*$1.identifier, $1.loc, parseContext);
  delete $1.identifier;
  }
| STRUCT STRUCT_TYPE {
  PARSER_TRACE << "type_specifier_nonarray Sturct "<< *$2.identifier << std::endl;
  $$ = CreateTypedNode(*$2.identifier, $1.loc, parseContext);
  delete $2.identifier;
  }
| anonymous_specifier {
  DCHECK($1->IsStructDeclNode());
  $$ = CreateTypedNode($1->ToStructDeclNode(), $1->loc(), parseContext);
  }
;

initializer
: assignment_expression {
  PARSER_TRACE << "initializer: assign_expression" << std::endl;
  InitializerNode* node = parseContext->Create(ASTNode::kInitializerNode, $1->loc())
      ->ToInitializerNode();
  node->AddExpression($1);
  $$ = node;
 }
| LEFT_BRACE initializer_list RIGHT_BRACE {
  PARSER_TRACE << "initializer: {initialize_list}" << std::endl;
  $$ = $2;
  $$->ToInitializerNode()->SetAsBlock();
 }
| LEFT_BRACE initializer_list COMMA RIGHT_BRACE {
  PARSER_TRACE << "{initializer_list, }" << std::endl;
  $$ = $2;
  $$->ToInitializerNode()->SetAsBlock();
 }
;

initializer_list
: initializer {
  PARSER_TRACE << "initializer_list" << std::endl;
  $$ = $1;
 }
| initializer_list COMMA initializer {
  PARSER_TRACE << "initializer_list, initializer" << std::endl;
  $$ = $1;
  DCHECK($$->IsInitializerNode());
  DCHECK($3->IsInitializerNode());
  InitializerNode* init = $$->ToInitializerNode();
  if ($3->ToInitializerNode()->IsBlock()) {
    init->AppendInitializerNode($3->ToInitializerNode());
  } else {
    init->AddExpression($3->ToInitializerNode()->DetchExpression());
  }
 }
;

/**
 * expression
 *
 */

expression
: assignment_expression {
  $$ = $1;
 }
| expression COMMA assignment_expression {
  $$ = $1;
  $$->AddSibling($3);
 }
;

// 整形表达式，数组下标访问使用此表达式， 在语法上与 expression 没有任何区别
// 这里将它单独拿出来是为了执行类型检查
integer_expression
: expression {
  // TODO( check value is integer)
  $$ = $1;
 }
;

// 常量表达式，主要用于数组大小的指定
// 这里将它单独拿出来是为了执行类型检查
constant_expression
: conditional_expression {
  // TODO( check value is constant)
  $$ = $1;
 }
;

variable_identifier
: IDENTIFIER {
  PARSER_TRACE << "variable_identifier " << std::endl;
  RefSymbolNode* symbol = CreateRefSymbolNode($1.identifier, $1.loc, parseContext);
  $$ = symbol;
  delete $1.identifier;
 }
;

primary_expression
: variable_identifier {
  PARSER_TRACE << "primary_expression: variable_identifier " << std::endl;
  $$ = $1;
 }
| BOOLCONST {
  PARSER_TRACE << "primary_expression bool const " << std::endl;
  ConstNode* constant = parseContext->Create(ASTNode::kConstNode, $1.loc)
      ->ToConstNode();
  constant->SetBoolean($1.b);
  $$ = constant;
  }
| CHARCONST {
  PARSER_TRACE << "primary_expression int const " << std::endl;
  ConstNode* constant = parseContext->Create(ASTNode::kConstNode, $1.loc)
      ->ToConstNode();
  constant->SetChar($1.c);
  $$ = constant;
  }
| INTCONST {
  PARSER_TRACE << "primary_expression int const " << std::endl;
  ConstNode* constant = parseContext->Create(ASTNode::kConstNode, $1.loc)
      ->ToConstNode();
  constant->SetInt32($1.i);
  $$ = constant;
  }
| UINTCONST {
  PARSER_TRACE << "primary_expression uint const " << std::endl;
  ConstNode* constant = parseContext->Create(ASTNode::kConstNode, $1.loc)
      ->ToConstNode();
  constant->SetUint32($1.u);
  $$ = constant;
  }
| FLOATCONST {
  PARSER_TRACE << "primary_expression float const " << std::endl;
  ConstNode* constant = parseContext->Create(ASTNode::kConstNode, $1.loc)
      ->ToConstNode();
  constant->SetFloat($1.d);
  $$ = constant;
  }
| STRING {
  PARSER_TRACE << "primary_expression string const " << std::endl;
  ConstNode* constant = parseContext->Create(ASTNode::kConstNode, $1.loc)
      ->ToConstNode();
  constant->SetString(*$1.string);
  $$ = constant;
  delete $1.string;
  }
;
| LEFT_PAREN expression RIGHT_PAREN {
  $$ = $2;
 }
;

/**
 * 前缀访问，例如数组下标访问，类成员访问，函数调用
 */ 
postfix_expression
: primary_expression {
  PARSER_TRACE << "postfix_expression " << std::endl;
  $$ = $1;
 }
| postfix_expression LEFT_BRACKET integer_expression RIGHT_BRACKET {
  PARSER_TRACE << "postfix_expression[integer] " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpIndex);
  $$ = node;
 }
| function_call {
  PARSER_TRACE << "postfix_expression: function_call " << std::endl;
  $$ = $1;
  }
| postfix_expression DOT IDENTIFIER {
  PARSER_TRACE << "postfix_expression dot " << std::endl;
  const SourceLoc& loc = $3.loc;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, loc)
      ->ToBinaryOpNode();
  FieldNode* field = parseContext->Create(ASTNode::kFieldNode, loc)->ToFieldNode();
  field->SetFieldName(*$3.identifier);
  node->SetOper1($1);
  node->SetOper2(field);
  node->SetOperator(kOpMember);
  $$ = node;
 }
| postfix_expression INC_OP {
  PARSER_TRACE << "postfix_expression++ " << std::endl;
  UnaryOpNode* node = parseContext->Create(ASTNode::kUnaryOpNode, $2.loc)
      ->ToUnaryOpNode();
  node->SetOper($1);
  node->SetOperator(kOpInc);
  $$ = node;
 }
| postfix_expression DEC_OP {
  PARSER_TRACE << "postfix_expression-- " << std::endl;
  UnaryOpNode* node = parseContext->Create(ASTNode::kUnaryOpNode, $2.loc)
      ->ToUnaryOpNode();
  node->SetOper($1);
  node->SetOperator(kOpDec);
  $$ = node;
 }
;

unary_expression
: postfix_expression {
  PARSER_TRACE << "unary_expression[postfix_expression]" << std::endl;
  $$= $1;
 }
| INC_OP unary_expression {
  PARSER_TRACE << "++unary_expression " << std::endl;
  UnaryOpNode* node = parseContext->Create(ASTNode::kUnaryOpNode, $1.loc)
      ->ToUnaryOpNode();
  node->SetOper($2);
  node->SetOperator(kOpInc);
  $$ = node;
 }
| DEC_OP unary_expression {
  PARSER_TRACE << "--unary_expression " << std::endl;
  UnaryOpNode* node = parseContext->Create(ASTNode::kUnaryOpNode, $1.loc)
      ->ToUnaryOpNode();
  node->SetOper($2);
  node->SetOperator(kOpDec);
  $$ = node;
 }
| unary_operator unary_expression {
  PARSER_TRACE << "unary_operator unary_expression " << std::endl;
  if ($1.op != kOpNull) {
    UnaryOpNode* node = parseContext->Create(ASTNode::kUnaryOpNode, $1.loc)
        ->ToUnaryOpNode();
    node->SetOper($2);
    node->SetOperator($1.op);
    $$ = node;
  } else {
    $$ = $2;
  }
 }
;

unary_operator
: PLUS  {
  PARSER_TRACE << "unary_operator + " << std::endl;
  $$.loc = $1.loc; $$.op = kOpNull;
 }
| DASH  {
  PARSER_TRACE << "unary_operator - " << std::endl;
  $$.loc = $1.loc; $$.op = kOpNegative;
  }
| BANG  {
  PARSER_TRACE << "unary_operator ! " << std::endl;
  $$.loc = $1.loc; $$.op = kOpLogicalNot;
  }
| TILDE {
  PARSER_TRACE << "unary_operator ~ " << std::endl;
  $$.loc = $1.loc; $$.op = kOpBitwiseNot;
  }
;

multiplicative_expression
: unary_expression {
  $$ = $1;
 }
| multiplicative_expression START unary_expression {
  PARSER_TRACE << "or_expression " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpMul);
  $$ = node;
 }
| multiplicative_expression SLASH unary_expression {
  PARSER_TRACE << "or_expression " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpDiv);
  $$ = node;
 }
| multiplicative_expression PERCENT unary_expression {
  PARSER_TRACE << "or_expression " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpMod);
  $$ = node;
 }
;

additive_expression
: multiplicative_expression {
  $$ = $1;
 }
| additive_expression PLUS multiplicative_expression {
  PARSER_TRACE << "operator(+) " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpAdd);
  $$ = node;
 }
| additive_expression DASH multiplicative_expression {
  PARSER_TRACE << "operator(-)" << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpSub);
  $$ = node;
 }
;

shift_expression
: additive_expression { $$ = $1; }
| shift_expression LSHIFT_OP additive_expression {
  PARSER_TRACE << "operator(<<) " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpBitwiseLshift);
  $$ = node;
 }
| shift_expression RSHIFT_OP additive_expression {
  PARSER_TRACE << "operator(>>) " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpBitwiseRshift);
  $$ = node;
 }
;

relational_expression
: shift_expression { $$ = $1; }
| relational_expression LEFT_ANGLE shift_expression {
  PARSER_TRACE << "operator(<) " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpLt);
  $$ = node;
 }
| relational_expression RIGHT_ANGLE shift_expression  {
  PARSER_TRACE << "operator(>) " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpGt);
  $$ = node;
 }
| relational_expression LE_OP shift_expression  {
  PARSER_TRACE << "relational_expression(<=) " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpLe);
  $$ = node;
 }
| relational_expression GE_OP shift_expression  {
  PARSER_TRACE << "relational_expression(>=) " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpGe);
  $$ = node;
 }
;

equality_expression
: relational_expression { $$ = $1; }
| equality_expression EQ_OP relational_expression  {
  PARSER_TRACE << "operator(==) " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpEq);
  $$ = node;
 }
| equality_expression NE_OP relational_expression {
  PARSER_TRACE << "operator(!=) " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpNe);
  $$ = node;
 }
;

and_expression
: equality_expression { $$ = $1; }
| and_expression AMPERSAND equality_expression {
  PARSER_TRACE << "and_expression " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpBitwiseAnd);
  $$ = node;
 }
;

exclusive_or_expression
: and_expression { $$ = $1; }
| exclusive_or_expression CARET and_expression {
  PARSER_TRACE << "operator(^) " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpBitwiseXor);
  $$ = node;
 }
;

inclusive_or_expression
: exclusive_or_expression { $$ = $1; }
| inclusive_or_expression VERTICAL exclusive_or_expression {
  PARSER_TRACE << "operator(|)" << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpBitwiseOr);
  $$ = node;
 }
;

logical_and_expression
: inclusive_or_expression { $$ = $1; }
| logical_and_expression LAND_OP inclusive_or_expression {
  PARSER_TRACE << "operator(&&)" << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpLogicalAnd);
  $$ = node;
 }
;

logical_xor_expression
: logical_and_expression { $$ = $1; }
| logical_xor_expression LXOR_OP logical_and_expression  {
  PARSER_TRACE << "operator(^^) " << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpLogicalXor);
  $$ = node;
 }
;

logical_or_expression
: logical_xor_expression { $$ = $1; }
| logical_or_expression LOR_OP logical_xor_expression  {
  PARSER_TRACE << "operator(||)" << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator(kOpLogicalOr);
  $$ = node;
 }
;

conditional_expression
: logical_or_expression { $$ = $1; }
| logical_or_expression QUESTION expression COLON assignment_expression {
 }
;

assignment_expression
: conditional_expression { $$ = $1; }
| unary_expression assignment_operator assignment_expression {
  PARSER_TRACE << "operator(assign)" << std::endl;
  BinaryOpNode* node = parseContext->Create(ASTNode::kBinaryOpNode, $2.loc)
      ->ToBinaryOpNode();
  node->SetOper1($1);
  node->SetOper2($3);
  node->SetOperator($2.op);
  $$ = node;
 }
;

assignment_operator
: EQUAL {$$.loc = $1.loc; $$.op = kOpAssign; }
| MUL_ASSIGN {$$.loc = $1.loc;$$.op = kOpMulAssign;}
| DIV_ASSIGN {$$.loc = $1.loc;$$.op = kOpDivAssign; }
| MOD_ASSIGN   {
  // (TODO) check type
  $$.loc = $1.loc;$$.op = kOpModAssign;
  }
| ADD_ASSIGN {$$.loc = $1.loc;$$.op = kOpAddAssign;}
| SUB_ASSIGN {$$.loc = $1.loc;$$.op = kOpSubAssign;}
| LSHIFT_ASSIGN  {$$.loc = $1.loc; $$.op = kOpLShiftAssign;}
| RSHIFT_ASSIGN {$$.loc = $1.loc; $$.op = kOpRShiftAssign;}
| AND_ASSIGN   {$$.loc = $1.loc; $$.op = kOpAndAssign;}
| XOR_ASSIGN   {$$.loc = $1.loc; $$.op = kOpXorAssign;}
| OR_ASSIGN    {$$.loc = $1.loc; $$.op = kOpOrAssign;}
;


/**
 * attributes
 * attributes 的使用包括以下几种情形
 * 情形1：某些特殊结构体的表述信息
 * struct VSOutput {
 *  vec4 position[semantic="POSITION" system="true"]
 * };
 * 情形2：uniform 的特殊属性
 * [uniform_func="SetWorld" default="identity"] uniform mat4 world;
 */
attributes : LEFT_BRACKET attribute_list RIGHT_BRACKET {
  PARSER_TRACE << "Attributes Declaration" << std::endl;
  $$ = $2;
}
;

attribute_list : IDENTIFIER EQUAL STRING {
  PARSER_TRACE << "attribute_list" << std::endl;
  AttributesNode* node  = parseContext->Create(ASTNode::kAttributesNode, $1.loc)
      ->ToAttributesNode();
  DCHECK(node != NULL);
  if (!node->Add(*$1.identifier, *$3.string)) {
    parseContext->ReportError($$->loc(), $$->GetErrorMessage());
  }
  delete $1.identifier;
  delete $3.string;
  $$ = node;
 }
| attribute_list  IDENTIFIER EQUAL STRING {
  PARSER_TRACE << "attribute_list" << std::endl;
  if (!$$->Add(*$2.identifier, *$4.string)) {
    parseContext->ReportError($$->loc(), $$->GetErrorMessage());
  }
  delete $2.identifier;
  delete $4.string;
 }
| attribute_list DEFAULT EQUAL STRING {
  PARSER_TRACE << "attribute_list" << std::endl;
  if (!$$->Add("default", *$4.string)) {
    parseContext->ReportError($$->loc(), $$->GetErrorMessage());
  }
  delete $4.string;
 }
;

