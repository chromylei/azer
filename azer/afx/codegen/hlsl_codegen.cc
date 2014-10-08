#include "azer/afx/codegen/hlsl_codegen.h"

#include <sstream>
#include <iostream>
#include <iomanip>

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "azer/afx/codegen/hlsl_util.h"
#include "azer/afx/codegen/tex_util.h"
#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/context.h"
#include "azer/afx/compiler/expression_validator.h"

namespace azer {
namespace afx {

// #define TRACE_HLSL_CODEGEN
#ifdef TRACE_HLSL_CODEGEN 
#define TRACE() LOG(ERROR) << __FUNCTION__
#else
#define TRACE()
#endif

namespace {

bool NeedBraceForOp(ASTNode* node);
std::string MapBuiltInFunc(const std::string& name);
std::string DumpParamList(FuncCallNode* func, int start);

/**
 * 带有 sampler 检查
 * 如果遇到 texture 类型，他的 sampler 也将作为参数 dump 出来
 */
std::string DumpParamListWithTexSampler(FuncCallNode* func, int start);
}  // namespace

// class BinaryOpNodeHLSLCodeGen
void BinaryOpNodeHLSLCodeGen::GenBasicOperatorCode(std::string* code) {
  BinaryOpNode* binary = node()->ToBinaryOpNode();
  std::stringstream ss;
  HLSLCodeGeneratorFactory factory;
  SnippetCodeGenerator codegen(&factory);
  if (NeedBraceForOp(node())) ss << "(";
  codegen.GenCode(binary->GetOper1());
  ss << codegen.GetCode();
  if (binary->GetOperator() == kOpMember) {
    ss << operator_str(binary->GetOperator());
    codegen.GenCode(binary->GetOper2());
    ss << codegen.GetCode();
  } else if (binary->GetOperator() == kOpIndex) {
    SnippetCodeGenerator codegen2(&factory);
    codegen2.GenCode(binary->GetOper2());
    ss << "[" << codegen2.GetCode() << "]";
  } else {
    ss << " " << operator_str(binary->GetOperator()) << " ";
    codegen.GenCode(binary->GetOper2());
    ss << codegen.GetCode();
  }

  if (NeedBraceForOp(node())) ss << ")";
  *code = ss.str();
}

bool BinaryOpNodeHLSLCodeGen::IsBasicOper() {
  return true;
}

void BinaryOpNodeHLSLCodeGen::GenMatrixMultiply(std::string* code) {
  BinaryOpNode* binary = node()->ToBinaryOpNode();
  std::stringstream ss;
  HLSLCodeGeneratorFactory factory;
  SnippetCodeGenerator codegen(&factory);
  codegen.GenCode(binary->GetOper1());
  ss << "mul(" << codegen.GetCode() << ", ";
  codegen.GenCode(binary->GetOper2());
  ss << codegen.GetCode() << ")";
  *code = ss.str();
}

void BinaryOpNodeHLSLCodeGen::GenMatrixMultiplyAndAssign(std::string* code) {
  BinaryOpNode* binary = node()->ToBinaryOpNode();
  std::stringstream ss;
  HLSLCodeGeneratorFactory factory;
  SnippetCodeGenerator codegen(&factory);
  codegen.GenCode(binary->GetOper1());
  ss << codegen.GetCode() << " = mul(" << codegen.GetCode() << ", ";
  codegen.GenCode(binary->GetOper2());
  ss << codegen.GetCode() << ")";
  *code = ss.str();
}

bool BinaryOpNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  DCHECK_EQ(node()->type(), ASTNode::kBinaryOpNode);
  BinaryOpNode* binary = node()->ToBinaryOpNode();
  DCHECK(binary->GetOper1() != NULL);
  DCHECK(binary->GetOper2() != NULL);
  TypePtr type1 = GetNodeType(binary->GetOper1());
  TypePtr type2 = GetNodeType(binary->GetOper2());
  if (binary->GetOperator() == kOpMul
      && (type1->IsMatrix() || type2->IsMatrix())) {
    GenMatrixMultiply(code);
  } else if (binary->GetOperator() == kOpMulAssign
             && (type1->IsMatrix() || type2->IsMatrix())) {
    GenMatrixMultiplyAndAssign(code);
  } else {
    GenBasicOperatorCode(code);
  }
  
  return false;
}

// class CaseNodeNodeHLSLCodeGen
bool CaseNodeNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  return true;
}

void CaseNodeNodeHLSLCodeGen::GenCodeEnd(std::string* code) {
  TRACE();
}

// class ConditionalOpNodeNodeHLSLCodeGen
bool ConditionalOpNodeNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  return false;
}

// class ConstNodeHLSLCodeGen
bool ConstNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  DCHECK_EQ(node()->type(), ASTNode::kConstNode);
  ConstNode* constant = node()->ToConstNode();
  std::stringstream ss;
  DCHECK(constant->GetResultType().get());
  switch (constant->GetResultType()->type()) {
    case kBoolean: ss << (constant->value()->GetBoolean() ? "true" : "false"); break;
    case kChar: ss << "'" << constant->value()->GetChar() << "'"; break;
    case kShort: ss << constant->value()->GetShort(); break;
    case kUint32: ss << constant->value()->GetUint32(); break;
    case kInt32: ss << constant->value()->GetInt32(); break;
    case kFloat:
      ss.flags(std::ios::showpoint);
      ss << constant->value()->GetFloat() << "f"; break;
    default: NOTREACHED(); break;
  }
  *code = ss.str();
  return false;
}

// class 
bool DeclarationNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  DCHECK_EQ(node()->type(), ASTNode::kDeclarationNode);
  DeclarationNode* decl = node()->ToDeclarationNode();

  if (!decl->GetType()->IsTexture()) {
    std::stringstream ss;
    ss << HLSLDumpFullType(decl->GetTypedNode()) << " ";
    *code = ss.str();
    return true;
  } else {
    return GenCodeForTexture(code);
  }
}

bool DeclarationNodeHLSLCodeGen::GenCodeForTexture(std::string* code) {
  std::stringstream ss;
  ASTNode* cur = node()->first_child();
  DeclarationNode* decl = node()->ToDeclarationNode();
  while (cur) {
    if (cur->IsSymbolNode()) {
      CHECK(cur->IsSymbolNode());
      SymbolNode* symbol = cur->ToSymbolNode();
      ss << HLSLTypeName(decl->GetType()) << " ";
      ss << symbol->symbolname();
      if (symbol->GetType()->IsArray()) {
        ss << DumpArraySpecifier(symbol->GetType());
      }

      ss << ";\n";
      ss << "SamplerState " << HLSLUniformTextureSamplerFullName(cur) << ";\n";
    } else {
      DCHECK(cur->IsAttributesNode());
    }
    cur = cur->next_sibling();
  }

  *code = ss.str();
  return false;
}

void DeclarationNodeHLSLCodeGen::GenCodeEnd(std::string* code) {
  if (node()->parent()->IsScopedNode() && node()->parent()->parent() == NULL) {
    code->append(";");
  }
  TRACE();
}

// class FieldNodeHLSLCodeGen
bool FieldNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  if (node()->parent()->type() == ASTNode::kBinaryOpNode) {
    return GenForFieldAccess(code);
  } else if (node()->parent()->type() == ASTNode::kStructDeclNode) {
    return GenForDecl(code);
  } else {
    NOTREACHED();
    return false;
  }
}

bool FieldNodeHLSLCodeGen::GenForFieldAccess(std::string* code) {
  FieldNode* field = node()->ToFieldNode();
  code->append(field->fieldname());
  return false;
}

bool FieldNodeHLSLCodeGen::GenForDecl(std::string* code) {
  DCHECK_EQ(node()->type(), ASTNode::kFieldNode);
  FieldNode* field = node()->ToFieldNode();
  std::stringstream ss;
  ss << HLSLTypeName(field->GetType()) << " " << field->fieldname();
  if (field->GetType()->IsArray()) {
    ss << DumpArraySpecifier(field->GetType());
  }

  // attributes
  AttributesNode* attrnode = field->attributes();
  if (attrnode) {
    ss << std::move(AttributeSupfix(field->fieldname(), attrnode));
  } else if (node()->parent()->ToStructDeclNode()->IsExchangeStruct()) {
    ss << ":" << StringToUpperASCII(field->fieldname()) << "";
  }

  ASTNode* pnode = field->parent();
  if (pnode->type() == ASTNode::kStructDeclNode) {
    ss << ";";
  } else if (pnode->type() == ASTNode::kFuncProtoNode &&
             field->next_sibling() != NULL) {
    ss << ",";
  }
  *code = ss.str();
  return false;
}

// FieldNodeHLSLCodeGen
bool ForLoopNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  ForLoopNode* loop = node()->ToForLoopNode();
  std::stringstream ss;
  HLSLCodeGeneratorFactory factory;
  SnippetCodeGenerator codegen(&factory);

  ss << "for(";
  if (loop->GetInitNode()) {
    codegen.GenCode(loop->GetInitNode());
    ss << codegen.GetCode();
  }
  ss << ";";
  if (loop->GetConditionNode()) {
    codegen.GenCode(loop->GetConditionNode());
    ss << codegen.GetCode();
  }
  ss << ";";
  if (loop->GetResetNode()) {
    codegen.GenCode(loop->GetResetNode());
    ss << codegen.GetCode();
  }
  ss << "){";
  codegen.GenCode(loop->GetStatements());
  ss << codegen.GetCode();
  ss << "}";
  *code = ss.str();
  return false;
}

// class FuncCallNodeHLSLCodeGen
bool FuncCallNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  std::stringstream ss;
  DCHECK(node()->GetContext());
  FuncCallNode* func = node()->ToFuncCallNode();
  if (func->IsBuiltIn()) {
    if (func->funcname() == "sample1D"
        || func->funcname() == "sample2D"
        || func->funcname() == "sample3D"
        || func->funcname() == "sampleCube") {
      ss << std::move(GenCodeForSample());
    } else if (func->funcname() == "load") {
      ss << std::move(GenCodeForTexLoad());
    } else {
      ss << MapBuiltInFunc(func->funcname()) << "(" << DumpParamList(func, 0) << ")";
    }
  } else {
    FuncDefNode* funcdef = func->GetFuncDefNode();
    DCHECK(NULL != funcdef);
    ss << PackagePrefix(funcdef->GetContext()) << funcdef->funcname()
       << "(" << DumpParamListWithTexSampler(func, 0) << ")";
  }
  *code = ss.str();
  return false;
}

std::string FuncCallNodeHLSLCodeGen::GenCodeForTexLoad() {
  FuncCallNode* func = node()->ToFuncCallNode();
  ASTNode* tex = func->GetParams()[0];
  std::string symbolname = HLSLRefferedTextureFullName(tex);

  std::stringstream ss;
  ss << symbolname << ".Load(" << DumpParamList(func, 1) << ");";
  return ss.str();
}

std::string FuncCallNodeHLSLCodeGen::GenCodeForSample() {
  std::stringstream ss;
  FuncCallNode* func = node()->ToFuncCallNode();
  CHECK_EQ(func->GetParams().size(), 2u);
  ASTNode* tex = func->GetParams()[0];
  ASTNode* coord = func->GetParams()[1];
  std::string symbolname = HLSLRefferedTextureFullName(tex);
  ss << symbolname << ".Sample(" << HLSLUniformTextureSamplerFullName(tex) << ", ";

  HLSLCodeGeneratorFactory factory;
  SnippetCodeGenerator codegen(&factory);
  codegen.GenCode(coord);
  ss << codegen.GetCode() << ")";
  return ss.str();
}

bool FuncCallTypeInitNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();

  std::stringstream ss;
  FuncCallTypeInitNode* func = node()->ToFuncCallTypeInitNode();
  TypePtr type = func->GetType();
  DCHECK(type.get() != NULL);
  ss << HLSLTypeName(type) << "(" << DumpParamList(func, 0) << ")";
  *code = ss.str();
  return false;
}

// class FuncDefNodeHLSLCodeGen
bool FuncDefNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  return true;
}

// class FuncProtoNodeHLSLCodeGen
bool FuncProtoNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  std::stringstream ss;
  DCHECK(node()->GetContext());
  FuncProtoNode* func = node()->ToFuncProtoNode();
  ss << HLSLDumpFullType(func->rettype()) << " "
     << PackagePrefix(node()->GetContext()) << func->funcname() << "(";

  for (auto iter = func->GetParams().begin();
       iter != func->GetParams().end(); ++iter) {
    if (iter != func->GetParams().begin()) {
      ss << ", ";
    }
    DCHECK((*iter)->IsParamNode());
    ParamNode* param = (*iter)->ToParamNode();
    ss << HLSLDumpFullType(param->GetTypedNode()) << " " << param->paramname();
    if (param->GetType()->IsTexture()) {
      ss << ", SamplerState " << HLSLUniformTextureSamplerFullName(param);
    }
  }

  *code = ss.str();
  return true;
}

void FuncProtoNodeHLSLCodeGen::GenCodeEnd(std::string* code) {
  TRACE();
  code->append(")");
  DCHECK(node()->parent());
  if (node()->parent()->IsFuncDefNode()) {
    FuncDefNode* func = node()->parent()->ToFuncDefNode();
    AttributesNode* attrnode = func->attributes();
    code->append(std::move(AttributeSupfix("", attrnode)));
    code->append(" ");
  }
}

// class IfElseNodeHLSLCodeGen
bool IfElseNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  IfElseNode* ifnode = node()->ToIfElseNode();
  HLSLCodeGeneratorFactory factory;
  SnippetCodeGenerator codegen(&factory);
  
  std::stringstream ss;
  codegen.GenCode(ifnode->GetSelectionExpr());
  ss << "if (" << codegen.GetCode() << ")";
  codegen.GenCode(ifnode->GetTrueStatement());
  ss << codegen.GetCode();
  if (ifnode->GetFalseStatement() != NULL) {
    codegen.GenCode(ifnode->GetFalseStatement());
    ss << " else " << codegen.GetCode();
  }
  *code = ss.str();
  return false;
}

bool InitializerNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  HLSLCodeGeneratorFactory factory;
  SnippetCodeGenerator codegen(&factory);
  ASTNode* cur = node()->first_child();
  DCHECK(cur != NULL);

  std::stringstream ss;
  if (cur && cur->next_sibling()) {
    ss << "{";
  }
  while (cur) {
    codegen.GenCode(cur);
    ss << codegen.GetCode();
    cur = cur->next_sibling();
    if (cur) ss << ", ";
  }

  cur = node()->first_child();
  if (cur && cur->next_sibling()) {
    ss << "}";
  }
  *code = ss.str();
  return false;
}

// class JumpNodeHLSLCodeGen
bool JumpNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  switch (node()->GetJumpType()) {
    case JumpNode::kBreak: code->append("break");
    case JumpNode::kContinue: code->append("continue");
    case JumpNode::kDiscard: code->append("discard");
    default: NOTREACHED();
  }
  return false;
}

// class RefSymbolHLSLCodeGen
bool RefSymbolHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  DCHECK(node()->GetContext());
  RefSymbolNode* refsymbol = node()->ToRefSymbolNode();
  SymbolNode* symbol = refsymbol->GetDeclNode();
  DCHECK(symbol != NULL);

  /**
   * 判断是否是全局变量，如果是全局变量，需要输出 package_name
   * 如果是局部变量则不需要
   */
  ScopedNode* scoped = symbol->GetScoped();
  if (scoped == NULL) {
    code->append(std::move(PackagePrefix(node()->GetContext())));
  }

  code->append(refsymbol->symbolname());
  return false;
}

// class ReturnHLSLCodeGen
bool ReturnHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  code->append("return ");
  return true;
}

// class ScopedNodeHLSLCodeGen
bool ScopedNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  if (node()->parent() != NULL) {
    *code = "{";
  }
  return true;
}

void ScopedNodeHLSLCodeGen::GenCodeEnd(std::string* code) {
  TRACE();
  if (node()->parent() != NULL) {
    *code = "}";
  }
}

// StatementNodeHLSLCodeGen
bool StatementNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  return true;
}
void StatementNodeHLSLCodeGen::GenCodeEnd(std::string* code) {
  TRACE();
  ASTNode* first = node()->first_child();
  if (first == NULL
    || first->type() == ASTNode::kForLoopNode
    || first->type() == ASTNode::kIfElseNode
    || first->type() == ASTNode::kWhileLoopNode) {
      // do nothing
  } else {
    code->append(";");
  }
}

// class StructDeclNodeHLSLCodeGen
bool StructDeclNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  DCHECK_EQ(node()->type(), ASTNode::kStructDeclNode);
  StructDeclNode* decl = node()->ToStructDeclNode();
  if (HasOnlyTextureField(decl)) {
    return false;
  }

  ParseContext* context = node()->GetContext();
  DCHECK(context);
  std::stringstream ss;
  ss << "struct ";
  
  ss << std::move(PackagePrefix(context)) << decl->struct_name() << " {";
  *code = ss.str();
  return true;
}

void StructDeclNodeHLSLCodeGen::GenCodeEnd(std::string* code) {
  TRACE();
  code->append("};");
}

// class SwitchNodeHLSLCodeGen
bool SwitchNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  code->append("}");
  return false;
}

// class SymbolNodeHLSLCodeGen
bool SymbolNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  DCHECK(node()->IsSymbolNode());
  SymbolNode* symbol = node()->ToSymbolNode();
  DCHECK(symbol->GetDeclarationNode() != NULL);
  DCHECK(!symbol->GetType()->IsTexture());
  return GenCommonCode(symbol, code);
}

bool SymbolNodeHLSLCodeGen::GenCommonCode(SymbolNode* symbol, std::string* code) {
  std::stringstream ss;
  ss << symbol->symbolname();
  if (symbol->GetType()->IsArray()) {
    ss << DumpArraySpecifier(symbol->GetType());
  }

  if (symbol->GetInitializer()) {
    HLSLCodeGeneratorFactory factory;
    SnippetCodeGenerator codegen(&factory);
    codegen.GenCode(symbol->GetInitializer());
    ss << " = " << codegen.GetCode();
  }

  if (symbol->GetDeclarationNode() && symbol->next_sibling()) {
    ss << ", ";
  }
  
  *code = ss.str();
  return false;
}


// class ActParamNodeHLSLCodeGen
bool ActParamNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  DCHECK(node()->IsActParamNode());
  return false;
}

// class UnaryOpNodeHLSLCodeGen
bool UnaryOpNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  std::stringstream ss;
  UnaryOpNode* unary = node();
  switch (unary->GetOperator()) {
    case kOpNull:
    default:
      if (NeedBraceForOp(unary)) ss << "(";
      ss << operator_str(unary->GetOperator());
  }
  *code = ss.str();
  return true;
}

void UnaryOpNodeHLSLCodeGen::GenCodeEnd(std::string* code) {
  TRACE();
  switch (node()->GetOperator()) {
    case kOpNull:
    default:
      if (NeedBraceForOp(node())) code->append(")");
  }
}

// class WhileLoopNodeHLSLCodeGen
bool WhileLoopNodeHLSLCodeGen::GenCodeBegin(std::string* code) {
  TRACE();
  DCHECK_EQ(node()->type(), ASTNode::kWhileLoopNode);
  WhileLoopNode* loop = node()->ToWhileLoopNode();

  std::stringstream ss;
  HLSLCodeGeneratorFactory factory;
  SnippetCodeGenerator codegen(&factory);

  if (loop->IsDoWhileLoop()) {
    ss << "do{";
    if (loop->GetStatements()) {
      codegen.GenCode(loop->GetStatements());
      ss << codegen.GetCode();
    } 
    ss << "}while";
    if (loop->GetExprNode()) {
      codegen.GenCode(loop->GetExprNode());
      ss << "(" << codegen.GetCode() << ")";
    } else {
      ss << "()";
    }
    ss << ";";
  } else {
    ss << "while";
    if (loop->GetExprNode()) {
      codegen.GenCode(loop->GetExprNode());
      ss << "(" << codegen.GetCode() << ")";
    } else {
      ss << "()";
    }
    ss << "{";
    if (loop->GetStatements()) {
      codegen.GenCode(loop->GetStatements());
      ss << codegen.GetCode();
    } 
    ss << "}";
  }
  *code = ss.str();
  return false;
}

// class HLSLCodeGeneratorFactory
HLSLCodeGeneratorFactory::~HLSLCodeGeneratorFactory() {
  for (auto iter = allocated_.begin(); iter != allocated_.end(); ++iter) {
    delete *iter;
  }
  allocated_.clear();
}

ASTCodeGenerator* HLSLCodeGeneratorFactory::CreateCodeGen(ASTNode* node) {
  ASTCodeGenerator* generator = NULL;
  switch (node->type()) {
    case ASTNode::kAttributesNode:
      generator = new AttributesNodeHLSLCodeGen(node); break;
    case ASTNode::kBinaryOpNode:
      generator = new BinaryOpNodeHLSLCodeGen(node); break;
    case ASTNode::kCaseNode:
      generator = new CaseNodeNodeHLSLCodeGen(node); break;
    case ASTNode::kConditionalOpNode:
      generator = new ConditionalOpNodeNodeHLSLCodeGen(node); break;
    case ASTNode::kConstNode:
      generator = new ConstNodeHLSLCodeGen(node); break;
    case ASTNode::kDeclarationNode:
      generator = new DeclarationNodeHLSLCodeGen(node); break;
    case ASTNode::kFieldNode:
      generator = new FieldNodeHLSLCodeGen(node); break;
    case ASTNode::kForLoopNode:
      generator = new ForLoopNodeHLSLCodeGen(node); break;
    case ASTNode::kFuncCallNode:
      generator = new FuncCallNodeHLSLCodeGen(node); break;
    case ASTNode::kFuncCallTypeInitNode:
      generator = new FuncCallTypeInitNodeHLSLCodeGen(node); break;
    case ASTNode::kFuncDefNode:
      generator = new FuncDefNodeHLSLCodeGen(node); break;
    case ASTNode::kFuncProtoNode:
      generator = new FuncProtoNodeHLSLCodeGen(node); break;
    case ASTNode::kIfElseNode:
      generator = new IfElseNodeHLSLCodeGen(node); break;
    case ASTNode::kInitializerNode:
      generator = new InitializerNodeHLSLCodeGen(node); break;
    case ASTNode::kJumpNode:
      generator = new JumpNodeHLSLCodeGen(node); break;
    case ASTNode::kNullNode:
      generator = new NullHLSLCodeGen(node); break;
    case ASTNode::kRefSymbolNode:
      generator = new RefSymbolHLSLCodeGen(node); break;
    case ASTNode::kReturnNode:
      generator = new ReturnHLSLCodeGen(node); break;
    case ASTNode::kScopedNode:
      generator = new ScopedNodeHLSLCodeGen(node); break;
    case ASTNode::kStatementNode:
      generator = new StatementNodeHLSLCodeGen(node); break;
    case ASTNode::kStructDeclNode:
      generator = new StructDeclNodeHLSLCodeGen(node); break;
    case ASTNode::kSwitchNode:
      generator = new SwitchNodeHLSLCodeGen(node); break;
    case ASTNode::kSymbolNode:
      generator = new SymbolNodeHLSLCodeGen(node); break;
    case ASTNode::kActParamNode:
      generator = new ActParamNodeHLSLCodeGen(node); break;
    case ASTNode::kUnaryOpNode:
      generator = new UnaryOpNodeHLSLCodeGen(node); break;
    case ASTNode::kWhileLoopNode: 
      generator = new WhileLoopNodeHLSLCodeGen(node); break;
    default:
      NOTREACHED(); return NULL;
  }
  allocated_.push_back(generator);
  return generator;
}

namespace {
bool NeedBraceForOp(ASTNode* node) {
  if (node->type() == ASTNode::kBinaryOpNode
      && node->ToBinaryOpNode()->GetOperator() == kOpMember) {
    return false;
  }

  if (node->parent() && node->parent()->IsStatementNode()) {
    return false;
  }

  if (node->parent() && (node->parent()->type() == ASTNode::kUnaryOpNode
                         || node->parent()->type() == ASTNode::kBinaryOpNode)) {
    return true;
  }

  return false;
}

std::string MapBuiltInFunc(const std::string& name) {
  return name;
}

std::string DumpParamListWithTexSampler(FuncCallNode* func, int start) {
  HLSLCodeGeneratorFactory factory;
  SnippetCodeGenerator codegen(&factory);
  std::stringstream ss;
  size_t num = func->GetParams().size();
  for (auto i = start; i < num; ++i) {
    ASTNode* node = func->GetParams()[i];
    if (i != start) {
      ss << ", ";
    }

    if (IsNodeTypeTexture(node)) {
      ss << HLSLRefferedTextureFullName(node);
      ss << ", " << HLSLUniformTextureSamplerFullName(node);
    } else {
      codegen.GenCode(node);
      ss << codegen.GetCode();
    }
  }
  return ss.str();
}

std::string DumpParamList(FuncCallNode* func, int start) {
  HLSLCodeGeneratorFactory factory;
  SnippetCodeGenerator codegen(&factory);
  std::stringstream ss;
  size_t num = func->GetParams().size();
  for (auto i = start; i < num; ++i) {
    ASTNode* node = func->GetParams()[i];
    if (i != start) {
      ss << ", ";
    }

    if (IsNodeTypeTexture(node)) {
      ss << HLSLRefferedTextureFullName(node);
    } else {
      codegen.GenCode(node);
      ss << codegen.GetCode();
    }
  }
  return ss.str();
}

}  // naemsapce
}  // namespace afx
}  // namespace azer

