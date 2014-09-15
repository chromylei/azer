#include "azer/afx/compiler/astnode.h"

#include <sstream>
#include "base/logging.h"
#include "base/strings/string16.h"
#include "base/strings/string_util.h"
#include "azer/afx/compiler/context.h"
#include "azer/afx/compiler/expression_validator.h"
#include "azer/afx/compiler/util.h"

namespace azer {
namespace afx {
ASTNode::ASTNode(const std::string& source_path, const SourceLoc& loc,
                 ASTNodeType type)
    : source_(source_path)
    , loc_(loc)
    , node_type_(type)
    , context_(NULL)
    , extra_(NULL) {
}

const char* ASTNode::name() {
  return ASTNodeName(type());
}

void ASTNode::RegisteSymbol(SymbolNode* symbol) {
  ASTNode* cur = this;
  while (cur) {
    if (cur->type() == kScopedNode) {
      ScopedNode* scoped = cur->ToScopedNode();
      scoped->RegisteSymbolLocally(symbol);
      break;
    }
    cur = cur->parent();
  }
}

void ASTNode::AddChildren(ASTNode* node) {
  ASTNode* cur = node;
  while (cur) {
    ASTNode* temp = cur;
    cur = cur->next_sibling();
    temp->reset_sibling();
    AddChild(temp);
  }
}

// class AttributesNode
bool AttributesNode::Add(const std::string& name, const std::string& field) {
  auto iter = fields_.find(name);
  if (iter != fields_.end()) {
    std::stringstream ss;
    ss << "[" << source() << ": " << lineno() << "] "
       << "dumplicatio item \"" << name << "\" in attributes " << name;
    SetError(ss.str());
    return false;
  }

  std::string output;
  ::base::TrimString(field, "\"", &output);
  fields_[name] = output;
  return true;
}

bool AttributesNode::HasAttr(const std::string& str) const {
  return fields_.find(str) != fields_.end();
}

BinaryOpNode::BinaryOpNode(const std::string& source, const SourceLoc& loc)
    : ExpressionNode(source, loc, kBinaryOpNode)
    , op_(kOpNull)
    , oper1_(NULL)
    , oper2_(NULL) {
}

void BinaryOpNode::SetOper1(ASTNode* oper) {
  DCHECK(oper1_ == NULL);
  oper1_ = oper;
  AddChildren(oper);
}

void BinaryOpNode::SetOper2(ASTNode* oper) {
  DCHECK(oper2_ == NULL);
  oper2_ = oper;
  AddChildren(oper);
}

void BinaryOpNode::SetOperator(Operator o) {
  DCHECK_EQ(op_, kOpNull);
  op_ = o;
}

ConditionalOpNode::ConditionalOpNode(const std::string& source,
                                     const SourceLoc& loc)
    : ASTNode(source, loc, kConditionalOpNode)
    , oper1_(NULL)
    , oper2_(NULL)
    , oper3_(NULL) {
}

void ConditionalOpNode::SetOper1(ASTNode* oper) {
  DCHECK(oper1_ == NULL);
  oper1_ = oper;
  AddChildren(oper);
}

void ConditionalOpNode::SetOper2(ASTNode* oper) {
  DCHECK(oper2_ == NULL);
  oper2_ = oper;
  AddChildren(oper);
}

void ConditionalOpNode::SetOper3(ASTNode* oper) {
  DCHECK(oper3_ == NULL);
  oper3_ = oper;
  AddChildren(oper);
}

void ConstNode::SetBoolean(bool b) {
  DCHECK(value_.get() == NULL);
  value_.reset(new Value(b));
}
void ConstNode::SetChar(char b) {
  DCHECK(value_.get() == NULL);
  value_.reset(new Value(b));
}
void ConstNode::SetShort(short s) {
  DCHECK(value_.get() == NULL);
  value_.reset(new Value(s));
}
void ConstNode::SetUint32(uint32 i32) {
  DCHECK(value_.get() == NULL);
  value_.reset(new Value(i32));
}
void ConstNode::SetInt32(int32 i) {
  DCHECK(value_.get() == NULL);
  value_.reset(new Value(i));
}
void ConstNode::SetFloat(float f) {
  DCHECK(value_.get() == NULL);
  value_.reset(new Value(f));
}
void ConstNode::SetDouble(double d) {
  DCHECK(value_.get() == NULL);
  value_.reset(new Value(d));
}
void ConstNode::SetString(const std::string& str) {
  NOTREACHED();
}

// class ExpressionNode
ExpressionNode::ExpressionNode(const std::string& source,
                               const SourceLoc& loc, ASTNodeType type)
    : ASTNode(source, loc, type) {
}

void ExpressionNode::SetResultType(TypePtr ptr) {
  DCHECK(value_.get() == NULL);
  value_.reset(new Value);
  value_->SetType(ptr);
}

TypePtr& ExpressionNode::GetResultType() {
  return value_->type();
}

const TypePtr& ExpressionNode::GetResultType() const {
  return value_->type();
}

void ExpressionNode::SetValue(const Value& value) {
  DCHECK(value_.get() == NULL);
  value_.reset(new Value(value));
}


// class FieldNode
FieldNode::FieldNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kFieldNode)
    , attributes_(NULL)
    , typed_node_(NULL) {
}

TypePtr& FieldNode::GetType() {
  DCHECK(typed_node_ != NULL);
  return typed_node_->GetType();
}

const TypePtr& FieldNode::GetType() const {
  DCHECK(typed_node_ != NULL);
  return typed_node_->GetType();
}

void FieldNode::SetTypedNode(ASTNode* node) {
  DCHECK(node != NULL && node->IsTypedNode());
  typed_node_ = node->ToTypedNode();
}

void FieldNode::SetAttributes(ASTNode* node) {
  DCHECK(attributes_ == NULL);
  DCHECK_EQ(node->type(), ASTNode::kAttributesNode);
  attributes_ = (AttributesNode*)node;
  AddChildren(node);
}

// class ForLoopNode
ForLoopNode::ForLoopNode(const std::string& source, const SourceLoc& loc)
      : ASTNode(source, loc, kForLoopNode)
      , init_node_(NULL)
      , condition_node_(NULL)
      , reset_node_(NULL)
      , statements_(NULL)
      , scoped_node_(NULL) {
}

void ForLoopNode::SetScopedNode(ASTNode* node) {
  DCHECK(scoped_node_ == NULL);
  scoped_node_ = node;
  AddChildren(node);
}

void ForLoopNode::SetInit(ASTNode* node) {
  DCHECK(init_node_ == NULL);
  DCHECK(scoped_node_ != NULL);
  init_node_ = node;
  scoped_node_->AddChildren(node);
}

void ForLoopNode::SetCondition(ASTNode* node) {
  DCHECK(condition_node_ == NULL);
  DCHECK(scoped_node_ != NULL);
  condition_node_ = node;
  scoped_node_->AddChildren(node);
}

void ForLoopNode::SetReset(ASTNode* node) {
  DCHECK(reset_node_ == NULL);
  DCHECK(scoped_node_ != NULL);
  reset_node_ = node;
  scoped_node_->AddChildren(node);
}

void ForLoopNode::SetStatements(ASTNode* node) {
  DCHECK(scoped_node_ != NULL);
  DCHECK(statements_ == NULL);
  statements_ = node;
  scoped_node_->AddChildren(node);
}

// class DeclarationNode
DeclarationNode::DeclarationNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kDeclarationNode)
    , typed_(NULL)
    , attributes_(NULL) {
}

void DeclarationNode::AddSymbol(ASTNode* node) {
  DCHECK(node->IsSymbolNode());
  AddChildren(node);
}

void DeclarationNode::SetAttributes(ASTNode* node) {
  DCHECK(attributes_ == NULL);
  DCHECK_EQ(node->type(), ASTNode::kAttributesNode);
  attributes_ = (AttributesNode*)node;
  if (!HasChildren()) {
    AddChildren(node);
  } else {
    node->InsertBefore(first_child());
  }
}

TypePtr& DeclarationNode::GetType() {
  DCHECK(typed_ != NULL);
  return typed_->GetType();
}

void DeclarationNode::SetTypedNode(ASTNode* node) {
  DCHECK(typed_ == NULL);
  DCHECK(node && node->IsTypedNode());
  typed_ = node->ToTypedNode();
}

// class FuncCallNode
FuncCallNode::FuncCallNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kFuncCallNode)
    , funcdef_node_(NULL) {
}

void FuncCallNode::AddParam(ASTNode* node) {
  DCHECK(node != NULL);
  AddChildren(node);
  params_.push_back(node);
}

const std::string& FuncCallNode::funcname() const {
  return func_name_;
}

bool FuncCallNode::IsBuiltIn() {
  DCHECK(GetFuncDefNode() != NULL);
  FuncDefNode* def = GetFuncDefNode();
  ParseContext* context = def->GetContext();
  return context->package() == "__builtin__";
}

const std::string& FuncCallNode::funcsync() {
  if (func_sync_.empty()) {
    std::stringstream ss;
    ss << funcname() << "(";
    for (auto iter = params_.begin(); iter != params_.end(); ++iter) {
      ASTNode* n = (*iter);
      TypePtr typeptr = GetNodeType(n);
      DCHECK(typeptr.get() != NULL);
      if (iter == params_.begin()) {
        ss << type_name(typeptr->type());
      } else {
        ss << "," << type_name(typeptr->type());
      }
    }
    ss << ")";
    func_sync_ = ss.str();
  }
  return func_sync_;
}

void FuncCallNode::SetFuncName(const std::string& name) {
  DCHECK(func_name_.empty());
  DCHECK(!name.empty());
  func_name_ = name;
}

void FuncCallNode::SetFuncDefNode(ASTNode* funcnode) {
  DCHECK_EQ(funcnode->type(), kFuncDefNode);
  DCHECK(funcdef_node_ == NULL);
  funcdef_node_ = funcnode->ToFuncDefNode();
}

void FuncCallTypeInitNode::SetType(TypePtr type) {
  DCHECK(type->IsBasicType());
  DCHECK(typeptr_.get() == NULL);
  typeptr_ = type;
}

// class FuncDefNode
FuncDefNode::FuncDefNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kFuncDefNode)
    , prototype_(NULL)
    , scoped_node_(NULL)
    , attributes_(NULL) {
}

void FuncDefNode::SetStatements(ASTNode* node) {
  DCHECK(scoped_node_ == NULL);
  DCHECK_EQ(node->type(), kScopedNode);
  scoped_node_ = node->ToScopedNode();
  AddChildren(node);
}

void FuncDefNode::SetAttributes(ASTNode* node) {
  DCHECK(attributes_ == NULL);
  DCHECK_EQ(node->type(), kAttributesNode);
  attributes_ = node->ToAttributesNode();
}

void FuncDefNode::SetProtoNode(ASTNode* node) {
  DCHECK(prototype_ == NULL);
  DCHECK(scoped_node_ != NULL);
  DCHECK_EQ(node->type(), kFuncProtoNode);
  prototype_ = node->ToFuncProtoNode();
  node->InsertBefore(scoped_node_);

  SourceLoc loc;
  loc.lineno = -1;
  for (auto iter = prototype_->GetParams().begin();
       iter != prototype_->GetParams().end(); ++iter) {
    ParamNode* param = (*iter)->ToParamNode();
    SymbolNode* symbol = CreateForFuncDefParams(param, context_);
    // need to register the symbol, which will be done at SyntaxValidator
    // scoped_node_->RegisteSymbolLocally(symbol);
    if (scoped_node_->HasChildren()) {
      symbol->InsertBefore(scoped_node_->first_child());
    } else {
      scoped_node_->AddChild(symbol);
    }
  }
}

FuncProtoNode* FuncDefNode::GetProtoNode() {
  DCHECK(prototype_ != NULL);
  return prototype_;
}

const std::string& FuncDefNode::funcname() {
  DCHECK(prototype_ != NULL);
  return prototype_->funcname();
}

// class FuncProtoNode
FuncProtoNode::FuncProtoNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kFuncProtoNode)
    , ret_typed_(NULL) {
}
void FuncProtoNode::SetFuncName(const std::string& name) {
  DCHECK(func_name_.empty());
  func_name_ = name;
}

void FuncProtoNode::SetRetTyped(ASTNode* node) {
  DCHECK(ret_typed_ == NULL);
  DCHECK(node && node->IsTypedNode());
  ret_typed_ = node->ToTypedNode();
}

void FuncProtoNode::AddParam(ASTNode* node) {
  DCHECK(node->IsParamNode());
  params_.push_back(node->ToParamNode());
}

const std::string& FuncProtoNode::funcsync() {
  if (func_sync_.empty()) {
    DCHECK(!funcname().empty());
    DCHECK(ret_typed_ && ret_typed_->GetType().get() != NULL);
    std::stringstream ss;
    ss << funcname() << "(";
    for (auto iter = params_.begin(); iter != params_.end(); ++iter) {
      if (iter == params_.begin()) {
        ss << type_name((*iter)->GetType()->type());
      } else {
        ss << "," << type_name((*iter)->GetType()->type());
      }
    }
    ss << ")";
    // ss << ";" << type_name(ret_type_->type());
    func_sync_ = std::move(ss.str());
  }
  return func_sync_;
}

// IfElseNode
IfElseNode::IfElseNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kIfElseNode)
    , expression_(NULL)
    , if_node_(NULL)
    , else_node_(NULL) {
}

void IfElseNode::SetSelectionExpr(ASTNode* node) {
  DCHECK(NULL == expression_);
  expression_ = node;
  AddChildren(node);
}

void IfElseNode::SetTrueStatements(ASTNode* node) {
  DCHECK(if_node_ == NULL);
  if_node_ = node;
  AddChildren(node);
}

void IfElseNode::SetFalseStatements(ASTNode* node) {
  DCHECK(else_node_ == NULL);
  else_node_ = node;
  AddChildren(node);
} 

// class InitializerNode
void InitializerNode::AddExpression(ASTNode* node) {
  DCHECK(node->type() == kUnaryOpNode
         || node->IsBinaryOpNode()
         || node->IsConditionalOpNode()
         || node->IsInitializerNode()
         || node->IsFuncCallNode()
         || node->IsConstNode());
  AddChildren(node);
}

void InitializerNode::AppendInitializerNode(ASTNode* node) {
  DCHECK(node->IsInitializerNode());
  DCHECK(node->ToInitializerNode()->IsBlock());
  AddSibling(node);
}

ASTNode* InitializerNode::DetchExpression() {
  return first_child();
}

// JumpNode
JumpNode::JumpNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kJumpNode)
    , jump_type_(kNotSpec) {
}

// ScopedNode
void ScopedNode::RegisteSymbolLocally(SymbolNode* node) {
  DCHECK(node->IsSymbolNode());
  symbols_.insert(std::make_pair(node->symbolname(), node));
}

SymbolNode* ScopedNode::LookupSymbol(const std::string& node) {
  auto iter = symbols_.find(node);
  if (iter != symbols_.end()) return iter->second;
  return LookupSymbolGlobally(node);
}

SymbolNode* ScopedNode::LookupSymbolLocally(const std::string& node) {
  auto iter = symbols_.find(node);
  if (iter != symbols_.end()) return iter->second;
  else return NULL;
}

SymbolNode* ScopedNode::LookupSymbolGlobally(const std::string& node) {
  ASTNode* cur = parent();
  while (cur) {
    if (cur->type() == kScopedNode) {
      return ((ScopedNode*)cur)->LookupSymbol(node);
    }
    cur = cur->parent();
  }

  return NULL;
}

// class ParamNode
ParamNode::ParamNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kParamNode)
    , attributes_(NULL)
    , typed_node_(NULL) {
}

TypePtr& ParamNode::GetType() {
  DCHECK(typed_node_ != NULL);
  return typed_node_->GetType();
}

const TypePtr& ParamNode::GetType() const {
  DCHECK(typed_node_ != NULL);
  return typed_node_->GetType();
}

void ParamNode::SetTypedNode(ASTNode* node) {
  DCHECK(node != NULL && node->IsTypedNode());
  typed_node_ = node->ToTypedNode();
}

void ParamNode::SetAttributes(ASTNode* node) {
  DCHECK(attributes_ == NULL);
  DCHECK_EQ(node->type(), ASTNode::kAttributesNode);
  attributes_ = (AttributesNode*)node;
  AddChildren(node);
}

// class RefSymbolNode
RefSymbolNode::RefSymbolNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kRefSymbolNode)
    , decl_node_(NULL) {
}

RefSymbolNode::RefSymbolNode(const std::string& source, const SourceLoc& loc,
                             ASTNodeType type)
    : ASTNode(source, loc, kRefSymbolNode)
    , decl_node_(NULL) {
}

void RefSymbolNode::SetDeclNode(SymbolNode* node) {
  DCHECK(decl_node_ == NULL);
  DCHECK(node->IsSymbolNode());
  decl_node_ = node;
}

// class StructDeclNode
StructDeclNode::StructDeclNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kStructDeclNode)
    , exchange_(false) {
}

bool StructDeclNode::HasField(const std::string& fieldname) const {
  return fields_.find(fieldname) != fields_.end();
}

FieldNode* StructDeclNode::GetField(const std::string& fieldname) const {
  auto iter = fields_.find(fieldname);
  if (iter != fields_.end()) {return iter->second;}
  else { return NULL;}
}

bool StructDeclNode::AddFields(ASTNode* node) {
  DCHECK_EQ(node->type(), kFieldNode);
  ASTNode* cur = node;
  while (cur) {
    FieldNode* field = cur->ToFieldNode();
    if (HasField(field->fieldname())) {
      std::stringstream ss;
      ss << "[" << source() << ": " << lineno() << "] dumplicatio item \""
         << field->fieldname() << "\" in structure " << field->GetType()->name();
      SetError(ss.str());
      return false;
    }
    fields_.insert(make_pair(field->fieldname(), field));
    fieldsvec_.push_back(field);
    cur = cur->next_sibling();
  }
  
  AddChildren(node);
  return true;
}

void StructDeclNode::SetStructName(const std::string& name) {
  DCHECK(struct_name_.empty());
  struct_name_ = name;
}

// class SymbolNode
SymbolNode::SymbolNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kSymbolNode)
    , typed_node_(NULL)
    , declared_(NULL)
    , initializer_(NULL) {
}

void SymbolNode::SetTypedNode(ASTNode* node) {
  DCHECK(node != NULL && node->IsTypedNode());
  typed_node_ = node->ToTypedNode();
  if (!HasChildren()) {
    AddChildren(node);
  } else {
    node->InsertBefore(first_child());
  }
}

void SymbolNode::SetDeclarationNode(ASTNode* node) {
  DCHECK(declared_== NULL);
  DCHECK_EQ(node->type(), kDeclarationNode);
  declared_= node->ToDeclarationNode();
}

void SymbolNode::SetInitializer(ASTNode* node) {
  DCHECK(initializer_ == NULL);
  DCHECK_EQ(node->type(), ASTNode::kInitializerNode);
  AddChildren(node);
  initializer_ = node;
}

void SymbolNode::SetSymbolName(const std::string& fieldname) {
  DCHECK(symbol_name_.empty());
  symbol_name_ = fieldname;
}
const std::string& SymbolNode::symbolname() {
  return symbol_name_;
}

TypePtr& SymbolNode::GetType() {
  DCHECK(typed_node_ != NULL);
  return typed_node_->GetType();
}

const TypePtr& SymbolNode::GetType() const {
  DCHECK(typed_node_ != NULL);
  return typed_node_->GetType();
}

// class TypedNode
TypedNode::TypedNode(const std::string& source, const SourceLoc& loc)
    : ASTNode(source, loc, kTypedNode)
    , decl_node_(NULL) {
}

void TypedNode::SetType(TypePtr type) {
  DCHECK(type_.get() == NULL);
  type_.reset(new Type(type->type(), type->storage_qualifier()));;
}

void TypedNode::SetBasicType(BasicType type) {
  DCHECK(type_.get() == NULL);
  type_.reset(new Type(type));
}

void TypedNode::SetStructType(const std::string& name) {
  DCHECK(type_.get() == NULL);
  type_.reset(new Type(kStructure));
  type_->SetName(name);
}

void TypedNode::SetAnonymouseStruct(StructDeclNode* decl) {
  DCHECK(type_.get() == NULL);
  DCHECK(decl_node_ == NULL);
  DCHECK(decl->IsStructDeclNode());
  type_.reset(new Type(kAnonymouseStructure));
  decl_node_ = decl;
}

void TypedNode::SetStructDecl(ASTNode* node) {
  DCHECK(type_.get() != NULL);
  DCHECK(decl_node_ == NULL);
  DCHECK(node->IsStructDeclNode());
  decl_node_ = node->ToStructDeclNode();
}

// class UnaryOpNode
void UnaryOpNode::SetOper(ASTNode* oper) {
  DCHECK(oper_ == NULL);
  oper_ = oper;
  AddChildren(oper);
}

void UnaryOpNode::SetOperator(Operator o) {
  DCHECK_EQ(op_, kOpNull);
  op_ = o;
}

// class WhileLoopNode
WhileLoopNode::WhileLoopNode(const std::string& source, const SourceLoc& loc)
      : ASTNode(source, loc, kWhileLoopNode)
      , do_while_style_(false)
      , expr_node_(NULL)
      , statements_(NULL) {
}

void WhileLoopNode::SetExprNode(ASTNode* node) {
  DCHECK(expr_node_ == NULL);
  expr_node_ = node;
  AddChildren(node);
}

void WhileLoopNode::SetStatements(ASTNode* node) {
  DCHECK(statements_ == NULL);
  statements_ = node;
  AddChildren(node);
}

// ASTNodeFactory
ASTNode* ASTNodeFactory::Create(ASTNode::ASTNodeType type, const std::string& source,
                                const SourceLoc& loc) {
  ASTNode* node = NULL;
  switch (type) {
    case ASTNode::kAttributesNode:
      node = new AttributesNode(source, loc); break;
    case ASTNode::kBinaryOpNode:
      node = new BinaryOpNode(source, loc); break;
    case ASTNode::kCaseNode:
      node = new CaseNode(source, loc); break;
    case ASTNode::kConditionalOpNode:
      node = new ConditionalOpNode(source, loc); break;
    case ASTNode::kConstNode:
      node = new ConstNode(source, loc); break;
    case ASTNode::kDeclarationNode:
      node = new DeclarationNode(source, loc); break;
    case ASTNode::kFieldNode:
      node = new FieldNode(source, loc); break;
    case ASTNode::kForLoopNode:
      node = new ForLoopNode(source, loc); break;
    case ASTNode::kFuncCallNode:
      node = new FuncCallNode(source, loc); break;
    case ASTNode::kFuncCallTypeInitNode:
      node = new FuncCallTypeInitNode(source, loc); break;
    case ASTNode::kFuncDefNode:
      node = new FuncDefNode(source, loc); break;
    case ASTNode::kFuncProtoNode:
      node = new FuncProtoNode(source, loc); break;
    case ASTNode::kIfElseNode:
      node = new IfElseNode(source, loc); break;
    case ASTNode::kInitializerNode:
      node = new InitializerNode(source, loc); break;
    case ASTNode::kJumpNode:
      node = new JumpNode(source, loc); break;
    case ASTNode::kNullNode:
      node = new NullNode(source, loc); break;
    case ASTNode::kPackageNode:
      node = new PackageNode(source, loc); break;
    case ASTNode::kParamNode:
      node = new ParamNode(source, loc); break;
    case ASTNode::kRefSymbolNode:
      node = new RefSymbolNode(source, loc); break;
    case ASTNode::kReturnNode:
      node = new ReturnNode(source, loc); break;
    case ASTNode::kScopedNode:
      node = new ScopedNode(source, loc); break;
    case ASTNode::kStatementNode:
      node = new StatementNode(source, loc); break;
    case ASTNode::kStructDeclNode:
      node = new StructDeclNode(source, loc); break;
    case ASTNode::kStructExternDeclNode:
      node = new StructExternDeclNode(source, loc); break;
    case ASTNode::kSwitchNode:
      node = new SwitchNode(source, loc); break;
    case ASTNode::kSymbolNode:
      node = new SymbolNode(source, loc); break;
    case ASTNode::kActParamNode:
      node = new ActParamNode(source, loc); break;
    case ASTNode::kTypedNode:
      node = new TypedNode(source, loc); break;
    case ASTNode::kUnaryOpNode:
      node = new UnaryOpNode(source, loc); break;
    case ASTNode::kWhileLoopNode: 
      node = new WhileLoopNode(source, loc); break;
    default:
      NOTREACHED(); return NULL;
  }

  return node;
}

const char* ASTNodeName(ASTNode::ASTNodeType type) {
  switch (type) {
    case ASTNode::kAttributesNode: return "AttributesNode";
    case ASTNode::kBinaryOpNode: return "BinaryOpNode";
    case ASTNode::kCaseNode: return "CaseNode";
    case ASTNode::kConditionalOpNode: return "ConditionalOpNode";
    case ASTNode::kConstNode: return "ConstNode";
    case ASTNode::kDeclarationNode: return "DeclarationNode";
    case ASTNode::kExpressionNode: return "ExpressionNode";
    case ASTNode::kFieldNode: return "FieldNode";
    case ASTNode::kForLoopNode: return "ForLoopNode";
    case ASTNode::kFuncCallNode: return "FuncCallNode";
    case ASTNode::kFuncCallTypeInitNode: return "FuncCallTypeInitNode";
    case ASTNode::kFuncDefNode: return "FuncDefNode";
    case ASTNode::kFuncProtoNode: return "FuncProtoNode";
    case ASTNode::kIfElseNode: return "IfElseNode";
    case ASTNode::kInitializerNode: return "InitializerNode";
    case ASTNode::kJumpNode: return "JumpNode";
    case ASTNode::kNullNode: return "NullNode";
    case ASTNode::kPackageNode: return "PackageNode";
    case ASTNode::kParamNode: return "ParamNode";
    case ASTNode::kRefSymbolNode: return "RefSymbolNode";
    case ASTNode::kReturnNode: return "ReturnNode";
    case ASTNode::kScopedNode: return "ScopedNode";
    case ASTNode::kStatementNode: return "StatementNode";
    case ASTNode::kStructDeclNode: return "StructDeclNode";
    case ASTNode::kStructExternDeclNode: return "StructExternDeclNode";
    case ASTNode::kSwitchNode: return "SwitchNode";
    case ASTNode::kSymbolNode: return "SymbolNode";
    case ASTNode::kActParamNode: return "ActParamNode";
    case ASTNode::kTypedNode: return "TypedNode";
    case ASTNode::kUnaryOpNode: return "UnaryOpNode";
    case ASTNode::kWhileLoopNode: return "WhileLoopNode";
    default: NOTREACHED(); return "";
  }
}

}  // namespace afx
}  // namespace azer
