#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "azer/afx/compiler/tree_node.h"
#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/value.h"
#include "azer/afx/compiler/loc.h"
#include "base/basictypes.h"

namespace azer {
namespace afx {

class ParseContext;
class AttributesNode;
class BinaryOpNode;
class CaseNode;
class ConditionalOpNode;
class ConstNode;
class DeclarationNode;
class ExpressionNode;
class FieldNode;
class ForLoopNode;
class FuncCallNode;
class FuncCallTypeInitNode;
class FuncDefNode;
class FuncProtoNode;
class IfElseNode;
class InitializerNode;
class JumpNode;
class NullNode;
class PackageNode;
class ParamNode;
class RefSymbolNode;
class ReturnNode;
class ScopedNode;
class StatementNode;
class StructDeclNode;
class StructExternDeclNode;
class SwitchNode;
class SymbolNode;
class TypedNode;
class UnaryOpNode;
class WhileLoopNode;
class ASTNodeFactory;

class ASTNode : public TreeNodeDownUp<ASTNode> {
 public:
  enum ASTNodeType {
    kAttributesNode = 1,
    kBinaryOpNode,
    kCaseNode,
    kConditionalOpNode,
    kConstNode,
    kDeclarationNode,
    kExpressionNode,
    kFieldNode,
    kForLoopNode,
    kFuncCallNode,
    kFuncCallTypeInitNode,
    kFuncDefNode,
    kFuncProtoNode,
    kIfElseNode,
    kInitializerNode,
    kJumpNode,
    kNullNode,
    kPackageNode,
    kParamNode,
    kRefSymbolNode,
    kReturnNode,
    kScopedNode,
    kStatementNode,
    kStructDeclNode,
    kStructExternDeclNode,
    kSwitchNode,
    kSymbolNode,
    kTypedNode,
    kUnaryOpNode,
    kWhileLoopNode,
  };

  ASTNode(const std::string& source_path, const SourceLoc& loc, ASTNodeType type);
  virtual ~ASTNode() {}

  // transform function
  virtual AttributesNode* ToAttributesNode() { NOTREACHED(); return NULL;}
  virtual BinaryOpNode* ToBinaryOpNode() { NOTREACHED(); return NULL;}
  virtual CaseNode* ToCaseNode() { NOTREACHED(); return NULL;}
  virtual ConditionalOpNode* ToConditionalOpNode() { NOTREACHED(); return NULL;}
  virtual ConstNode* ToConstNode() { NOTREACHED(); return NULL;}
  virtual DeclarationNode* ToDeclarationNode() { NOTREACHED(); return NULL;}
  virtual ExpressionNode* ToExpressionNode() { NOTREACHED(); return NULL;}
  virtual FieldNode* ToFieldNode() { NOTREACHED(); return NULL;}
  virtual ForLoopNode* ToForLoopNode() { NOTREACHED(); return NULL;}
  virtual FuncCallNode* ToFuncCallNode() { NOTREACHED(); return NULL;}
  virtual FuncCallTypeInitNode* ToFuncCallTypeInitNode() { NOTREACHED(); return NULL;}
  virtual FuncDefNode* ToFuncDefNode() { NOTREACHED(); return NULL;}
  virtual FuncProtoNode* ToFuncProtoNode() { NOTREACHED(); return NULL;}
  virtual IfElseNode* ToIfElseNode() { NOTREACHED(); return NULL;}
  virtual InitializerNode* ToInitializerNode() { NOTREACHED(); return NULL;}
  virtual JumpNode* ToJumpNode() { NOTREACHED(); return NULL;}
  virtual NullNode* ToNullNode() { NOTREACHED(); return NULL;}
  virtual PackageNode* ToPackageNode() { NOTREACHED(); return NULL;}
  virtual ParamNode* ToParamNode() { NOTREACHED(); return NULL;}
  virtual RefSymbolNode* ToRefSymbolNode() { NOTREACHED(); return NULL;}
  virtual ReturnNode* ToReturnNode() { NOTREACHED(); return NULL;}
  virtual ScopedNode* ToScopedNode() { NOTREACHED(); return NULL;}
  virtual StatementNode* ToStatementNode() { NOTREACHED(); return NULL;}
  virtual StructDeclNode* ToStructDeclNode() { NOTREACHED(); return NULL;}
  virtual StructExternDeclNode* ToStructExternDeclNode() { NOTREACHED(); return NULL;}
  virtual SwitchNode* ToSwitchNode() { NOTREACHED(); return NULL;}
  virtual SymbolNode* ToSymbolNode() { NOTREACHED(); return NULL;}
  virtual TypedNode* ToTypedNode() { NOTREACHED(); return NULL;}
  virtual UnaryOpNode* ToUnaryOpNode() { NOTREACHED(); return NULL;}
  virtual WhileLoopNode* ToWhileLoopNode() { NOTREACHED(); return NULL;}

  virtual bool IsAttributesNode() {return false;}
  virtual bool IsBinaryOpNode() {return false;}
  virtual bool IsCaseNode() {return false;}
  virtual bool IsConditionalOpNode() {return false;}
  virtual bool IsConstNode() {return false;}
  virtual bool IsDeclarationNode() {return false;}
  virtual bool IsExpressionNode() {return false;}
  virtual bool IsFieldNode() {return false;}
  virtual bool IsForLoopNode() {return false;}
  virtual bool IsFuncCallNode() {return false;}
  virtual bool IsFuncCallTypeInitNode() {return false;}
  virtual bool IsFuncDefNode() {return false;}
  virtual bool IsFuncProtoNode() {return false;}
  virtual bool IsIfElseNode() {return false;}
  virtual bool IsInitializerNode() {return false;}
  virtual bool IsJumpNode() {return false;}
  virtual bool IsNullNode() {return false;}
  virtual bool IsPackageNode() {return false;}
  virtual bool IsParamNode() { return false;}
  virtual bool IsRefSymbolNode() {return false;}
  virtual bool IsReturnNode() {return false;}
  virtual bool IsScopedNode() {return false;}
  virtual bool IsStatementNode() {return false;}
  virtual bool IsStructDeclNode() {return false;}
  virtual bool IsSwitchNode() {return false;}
  virtual bool IsSymbolNode() {return false;}
  virtual bool IsTypedNode() {return false;}
  virtual bool IsUnaryOpNode() {return false;}
  virtual bool IsWhileLoopNode() {return false;}
  
  const std::string& source() const { return source_;}
  int lineno() const { return loc_.lineno;}
  const SourceLoc& loc() const { return loc_;}
  const std::string& GetErrorMessage() const { return error_msg_;}

  ASTNodeType type() const { return node_type_;}
  void RegisteSymbol(SymbolNode* node);
  void AddChildren(ASTNode* node);

  ParseContext* GetContext() { return context_;}
  void* extra() { return extra_;}
  void set_extra(void* v) { extra_ = v;}
  const char* name();
 protected:
  void SetError(const std::string& errmsg) { error_msg_ = errmsg;}
  std::string error_msg_;
  std::string source_;
  SourceLoc loc_;
  ASTNodeType node_type_;

  friend class ParseContext;
  friend class ASTNodeFactory;
  ParseContext* context_;
 private:
  void* extra_;
  DISALLOW_COPY_AND_ASSIGN(ASTNode);
};

const char* ASTNodeName(ASTNode::ASTNodeType type);
typedef std::vector<ASTNode*> ASTNodeVec;
typedef std::map<std::string, ASTNode*> ASTNodeDict;

class AttributesNode : public ASTNode {
 public:
  AttributesNode(const std::string& source, const SourceLoc& loc)
      : ASTNode(source, loc, kAttributesNode) {
  }
  virtual AttributesNode* ToAttributesNode() OVERRIDE { return this;}
  virtual bool IsAttributesNode() OVERRIDE { return true;}
  
  typedef std::map<std::string, std::string> AttributesDict;
  bool Add(const std::string& name, const std::string& field);
  const AttributesDict& GetDict() const { return fields_;}
  bool HasAttr(const std::string& str) const;
  std::string GetAttrValue(const std::string& str) const {
    auto iter = fields_.find(str);
    if (iter != fields_.end()) return iter->second;
    else return "";
  }
 private:
  AttributesDict fields_;
  DISALLOW_COPY_AND_ASSIGN(AttributesNode);
};

/**
 * 此类表示一个表达式，最重要的功能是它具有一个返回值
 * BinaryOpNode, ConstNode, UnaryOpNode 都是他的子类
 */
class ExpressionNode : public ASTNode {
 public:
  ExpressionNode(const std::string& source, const SourceLoc& loc, ASTNodeType type);
  virtual bool IsExpressionNode() OVERRIDE { return true;}

  void SetResultType(TypePtr ptr);
  TypePtr& GetResultType();
  const TypePtr& GetResultType() const;

  void SetValue(const Value& value);
  ValuePtr& value() { return value_;}
  const ValuePtr& value() const { return value_;}
 protected:
  ValuePtr value_;
  DISALLOW_COPY_AND_ASSIGN(ExpressionNode);
};

/**
 * 双目运算符操作
 *
 */
class BinaryOpNode : public ExpressionNode {
 public:
  BinaryOpNode(const std::string& source, const SourceLoc& loc);
  virtual BinaryOpNode* ToBinaryOpNode() OVERRIDE { return this;}
  virtual bool IsBinaryOpNode() OVERRIDE { return true;}
  void SetOper1(ASTNode* oper);
  void SetOper2(ASTNode* oper);
  void SetOperator(Operator o);
  
  ASTNode* GetOper1() { return oper1_;}
  ASTNode* GetOper2() { return oper2_;}
  Operator GetOperator() const { return op_;}
 private:

  Operator op_;
  ASTNode *oper1_, *oper2_;
  DISALLOW_COPY_AND_ASSIGN(BinaryOpNode);
};

class CaseNode : public ASTNode {
 public:
  CaseNode(const std::string& source, const SourceLoc& loc)
      : ASTNode(source, loc, kCaseNode)
      , default_node_(false)
      , express_node_(NULL){
  }
  virtual CaseNode* ToCaseNode() OVERRIDE { return this;}
  virtual bool IsCaseNode() OVERRIDE { return true;}

  void SetExpressionNode(ASTNode* node) {}
  void SetAsDefaultNode() { default_node_ = true;}
 private:
  ASTNode* express_node_;
  bool default_node_;
  DISALLOW_COPY_AND_ASSIGN(CaseNode);
};

class ConditionalOpNode : public ASTNode {
 public:
  ConditionalOpNode(const std::string& source, const SourceLoc& loc);
  virtual ConditionalOpNode* ToConditionalOpNode() OVERRIDE { return this;}
  virtual bool IsConditionalOpNode() OVERRIDE { return true;}
  void SetOper1(ASTNode* oper);
  void SetOper2(ASTNode* oper);
  void SetOper3(ASTNode* oper);

  ASTNode* GetOper1() { return oper1_;}
  ASTNode* GetOper2() { return oper2_;}
  ASTNode* GetOper3() { return oper3_;}
 private:
  ASTNode *oper1_, *oper2_, *oper3_;
  DISALLOW_COPY_AND_ASSIGN(ConditionalOpNode);
};

/**
 * 常量结点(并非语义上的常量，而是数字，字符串的值)
 */
class ConstNode : public ExpressionNode {
 public:
  ConstNode(const std::string& source, const SourceLoc& loc)
      : ExpressionNode(source, loc, kConstNode) {
  }
  virtual ConstNode* ToConstNode() OVERRIDE { return this;}
  virtual bool IsConstNode() OVERRIDE { return true;}
  void SetBoolean(bool b);
  void SetChar(char b);
  void SetShort(short s);
  void SetUint32(uint32 i32);
  void SetInt32(int32 i);
  void SetFloat(float f);
  void SetDouble(double d);
  void SetString(const std::string& str);
 private:
  DISALLOW_COPY_AND_ASSIGN(ConstNode);
};

/**
 * class DeclarationNode
 */
class DeclarationNode : public ASTNode {
 public:
  DeclarationNode(const std::string& source, const SourceLoc& loc);
  virtual DeclarationNode* ToDeclarationNode() OVERRIDE { return this;}
  virtual bool IsDeclarationNode() OVERRIDE { return true;}

  void AddSymbol(ASTNode* node);
  void SetAttributes(ASTNode* node);
  AttributesNode* attributes() { return attributes_;}
  TypePtr& GetType();
  void SetTypedNode(ASTNode* node);
  TypedNode* GetTypedNode() { return typed_;}
 private:
  TypedNode* typed_;
  AttributesNode* attributes_;
  DISALLOW_COPY_AND_ASSIGN(DeclarationNode);
};

class FieldNode : public ASTNode {
 public:
  FieldNode(const std::string& source, const SourceLoc& loc);
  virtual FieldNode* ToFieldNode() OVERRIDE { return this;}
  virtual bool IsFieldNode() OVERRIDE { return true;}

  const std::string& fieldname() { return fieldname_;}
  void SetFieldName(const std::string& v) { fieldname_ = v;}
  void SetAttributes(ASTNode* node);
  void SetTypedNode(ASTNode* node);
  AttributesNode* attributes() { return attributes_;}
  
  TypePtr& GetType();
  const TypePtr& GetType() const;
  TypedNode* GetTypedNode() { return typed_node_;}
 private:
  TypedNode* typed_node_;
  std::string fieldname_;
  AttributesNode* attributes_;
  DISALLOW_COPY_AND_ASSIGN(FieldNode);
};

class ForLoopNode : public ASTNode {
 public:
  ForLoopNode(const std::string& source, const SourceLoc& loc);
  virtual ForLoopNode* ToForLoopNode() OVERRIDE { return this;}
  virtual bool IsForLoopNode() OVERRIDE { return true;}

  void SetInit(ASTNode* node);
  void SetCondition(ASTNode* node);
  void SetReset(ASTNode* node);
  void SetStatements(ASTNode* node);
  void SetScopedNode(ASTNode* node);

  ASTNode* GetInitNode() { return init_node_;}
  ASTNode* GetConditionNode() { return condition_node_;}
  ASTNode* GetResetNode() { return reset_node_;}
  ASTNode* GetStatements() { return statements_;}
 private:
  ASTNode* init_node_;
  ASTNode* condition_node_;
  ASTNode* reset_node_;
  ASTNode* statements_;
  ASTNode* scoped_node_;
  DISALLOW_COPY_AND_ASSIGN(ForLoopNode);
};

class FuncCallNode : public ASTNode {
 public:
  FuncCallNode(const std::string& source, const SourceLoc& loc);
  virtual FuncCallNode* ToFuncCallNode() OVERRIDE { return this;}
  virtual bool IsFuncCallNode() OVERRIDE { return true;}
  const std::string& funcname() const;
  const std::string& funcsync();

  void SetFuncDefNode(ASTNode* funcnode);
  void AddParam(ASTNode* node);
  void SetFuncName(const std::string& name);

  ASTNodeVec& GetParams() { return params_;}
  const ASTNodeVec& GetParams() const { return params_;}
  FuncDefNode* GetFuncDefNode() { return funcdef_node_;}
  bool IsBuiltIn();
 protected:
  FuncCallNode(const std::string& source, const SourceLoc& loc, ASTNodeType type)
      : ASTNode(source, loc, type), funcdef_node_(NULL) {}
 private:
  FuncDefNode* funcdef_node_;
  std::vector<ASTNode*> params_;
  std::string func_name_;
  std::string func_sync_;
  DISALLOW_COPY_AND_ASSIGN(FuncCallNode);
};

class FuncCallTypeInitNode : public FuncCallNode {
 public:
  FuncCallTypeInitNode(const std::string& source, const SourceLoc& loc)
      : FuncCallNode(source, loc, kFuncCallTypeInitNode) {
  }

  virtual FuncCallTypeInitNode* ToFuncCallTypeInitNode() OVERRIDE { return this;}
  virtual bool IsFuncCallTypeInitNode() OVERRIDE { return true;}
  void SetType(TypePtr type);
  TypePtr GetType() { return typeptr_;}
 private:
  std::shared_ptr<Type> typeptr_;
  DISALLOW_COPY_AND_ASSIGN(FuncCallTypeInitNode);
};

class FuncDefNode : public ASTNode {
 public:
  FuncDefNode(const std::string& source, const SourceLoc& loc);
  virtual FuncDefNode* ToFuncDefNode() OVERRIDE { return this;}
  virtual bool IsFuncDefNode() OVERRIDE { return true;}
  void SetProtoNode(ASTNode* node);
  void SetStatements(ASTNode* node);
  void SetAttributes(ASTNode* node);
  FuncProtoNode* GetProtoNode();
  AttributesNode* GetAttributes() { return attributes_;}

  const std::string& funcname();
 private:
  FuncProtoNode* prototype_;
  ScopedNode* scoped_node_;
  AttributesNode* attributes_;
  DISALLOW_COPY_AND_ASSIGN(FuncDefNode);
};

class FuncProtoNode : public ASTNode {
 public:
  FuncProtoNode(const std::string& source, const SourceLoc& loc);
  virtual FuncProtoNode* ToFuncProtoNode() OVERRIDE { return this;}
  virtual bool IsFuncProtoNode() OVERRIDE { return true;}
  
  const std::string& funcname() const { return func_name_;}
  /**
   * 计算函数签名（包含函数名及参数类型名)
   * Note: 其中不包含函数返回值，这与 C++ 函数重载的规则一致
   * 具体原因可以参考 C++ 资料，从实现的角度看，在通过函数签名查找函数时
   * FuncCallNode 仅仅拥有函数的参数信息，而不知道返回值信息，只能通过遍历查找
   */
  const std::string& funcsync();
  const std::vector<FieldNode*>& GetParams() const { return params_;}
  TypedNode* rettype() { return ret_typed_;}

  void SetFuncName(const std::string& name);
  void SetRetTyped(ASTNode* node);
  void AddParam(ASTNode* node);
 private:
  std::string func_name_;
  std::string func_sync_;
  TypedNode* ret_typed_;
  std::vector<FieldNode*> params_;
  DISALLOW_COPY_AND_ASSIGN(FuncProtoNode);
};

class InitializerNode : public ASTNode {
 public:
  InitializerNode(const std::string& source, const SourceLoc& loc)
      : ASTNode(source, loc, kInitializerNode)
      , block_(false) {
  }
  virtual InitializerNode* ToInitializerNode() OVERRIDE { return this;}
  virtual bool IsInitializerNode() OVERRIDE { return true;}
  void AddExpression(ASTNode* node);
  void AppendInitializerNode(ASTNode* node);
  ASTNode* DetchExpression();
  TypePtr& GetType();

  void SetAsBlock() { block_ = true;}
  bool IsBlock() const { return block_;}
 private:
  bool block_;
  DISALLOW_COPY_AND_ASSIGN(InitializerNode);
};

class IfElseNode : public ASTNode {
 public:
  IfElseNode(const std::string& source, const SourceLoc& loc);
  virtual IfElseNode* ToIfElseNode() OVERRIDE { return this;}
  virtual bool IsIfElseNode() OVERRIDE { return true;}
  void SetSelectionExpr(ASTNode* node);
  void SetTrueStatements(ASTNode* node);
  void SetFalseStatements(ASTNode* node);

  ASTNode* GetSelectionExpr()  { return expression_;}
  ASTNode* GetTrueStatement()  { return if_node_;}
  ASTNode* GetFalseStatement() { return else_node_;}
 private:
  ASTNode* expression_;
  ASTNode* if_node_;
  ASTNode* else_node_;
  DISALLOW_COPY_AND_ASSIGN(IfElseNode);
};

class JumpNode : public ASTNode {
 public:
  enum JumpType {
    kNotSpec,
    kBreak,
    kContinue,
    kDiscard,
  };

  JumpNode(const std::string& source, const SourceLoc& loc);
  virtual JumpNode* ToJumpNode() OVERRIDE { return this;}
  virtual bool IsJumpNode() OVERRIDE { return true;}
  void SetJumpType(JumpType t) { jump_type_ = t;}
  JumpType GetJumpType() const { return jump_type_;}
 private:
  JumpType jump_type_;
  DISALLOW_COPY_AND_ASSIGN(JumpNode);
};

class NullNode : public ASTNode {
 public:
  NullNode(const std::string& source, const SourceLoc& loc)
      : ASTNode(source, loc, kNullNode) {
  }
  virtual NullNode* ToNullNode() OVERRIDE { return this;}
  virtual bool IsNullNode() OVERRIDE { return true;}
 private:
  DISALLOW_COPY_AND_ASSIGN(NullNode);
};

class PackageNode : public ASTNode {
 public:
  PackageNode(const std::string& source, const SourceLoc& loc)
      : ASTNode(source, loc, kPackageNode) {
  }
  virtual PackageNode* ToPackageNode() OVERRIDE { return this;}
  virtual bool IsPackageNode() OVERRIDE { return true;}

  const std::string& packagename() { return packagename_;}
 private:
  std::string packagename_;
  DISALLOW_COPY_AND_ASSIGN(PackageNode);
};

class ParamNode : public ASTNode {
 public:
  ParamNode(const std::string& source, const SourceLoc& loc);
  virtual ParamNode* ToParamNode() OVERRIDE { return this;}
  virtual bool IsParamNode() OVERRIDE { return true;}

  const std::string& fieldname() { return fieldname_;}
  void SetFieldName(const std::string& v) { fieldname_ = v;}
  void SetAttributes(ASTNode* node);
  void SetTypedNode(ASTNode* node);
  AttributesNode* attributes() { return attributes_;}
  
  TypePtr& GetType();
  const TypePtr& GetType() const;
  TypedNode* GetTypedNode() { return typed_node_;}
 private:
  TypedNode* typed_node_;
  std::string fieldname_;
  AttributesNode* attributes_;
  DISALLOW_COPY_AND_ASSIGN(ParamNode);
};

class RefSymbolNode : public ASTNode {
 public:
  RefSymbolNode(const std::string& source, const SourceLoc& loc);
  virtual RefSymbolNode* ToRefSymbolNode() OVERRIDE { return this;}
  virtual bool IsRefSymbolNode() OVERRIDE { return true;}

  void SetDeclNode(SymbolNode* node);
  SymbolNode* GetDeclNode() {return decl_node_;}
  void SetSymbolName(const std::string& name) { symbol_name_ = name;}
  void SetPackageName(const std::string& name) { package_name_ = name;}
  const std::string& symbolname() const { return symbol_name_;}
  virtual bool IsExternalSymbol() const { return false;}
 protected:
  RefSymbolNode(const std::string& source, const SourceLoc& loc, ASTNodeType type);
 private:
  SymbolNode* decl_node_;
  std::string package_name_;
  std::string symbol_name_;
  DISALLOW_COPY_AND_ASSIGN(RefSymbolNode);
};

class ReturnNode : public ASTNode {
 public:
  ReturnNode(const std::string& source, const SourceLoc& loc)
      : ASTNode(source, loc, kReturnNode)
      , expr_node_(NULL) {
  }
  virtual ReturnNode* ToReturnNode() OVERRIDE { return this;}
  virtual bool IsReturnNode() OVERRIDE { return true;}
  void SetExprNode(ASTNode* node) {expr_node_ = node; AddChildren(node);}
 private:
  ASTNode* expr_node_;
  DISALLOW_COPY_AND_ASSIGN(ReturnNode);
};

class ScopedNode : public ASTNode {
 public:
  ScopedNode(const std::string& source, const SourceLoc& loc)
      : ASTNode(source, loc, kScopedNode) {
  }
  virtual ScopedNode* ToScopedNode() OVERRIDE { return this;}
  virtual bool IsScopedNode() OVERRIDE { return true;}
  SymbolNode* LookupSymbol(const std::string& node);
  SymbolNode* LookupSymbolLocally(const std::string& node);
  void RegisteSymbolLocally(SymbolNode* node);
 private:
  SymbolNode* LookupSymbolGlobally(const std::string& node);
  std::map<std::string, SymbolNode*> symbols_;
  DISALLOW_COPY_AND_ASSIGN(ScopedNode);
};

class StatementNode : public ASTNode {
 public:
  StatementNode(const std::string& source, const SourceLoc& loc)
      : ASTNode(source, loc, kStatementNode) {
  }
  virtual StatementNode* ToStatementNode() OVERRIDE { return this;}
  virtual bool IsStatementNode() OVERRIDE { return true;}
 private:
  DISALLOW_COPY_AND_ASSIGN(StatementNode);
};

class StructDeclNode : public ASTNode {
 public:
  StructDeclNode(const std::string& source, const SourceLoc& loc);
  virtual StructDeclNode* ToStructDeclNode() OVERRIDE {return this;}
  virtual bool IsStructDeclNode() OVERRIDE {return true;}

  bool AddFields(ASTNode* field);
  const std::string& struct_name() const { return struct_name_;}
  void SetStructName(const std::string& name);
  void SetExchangeStruct() { exchange_ = true;}
  bool IsExchangeStruct()  { return exchange_;}

  const std::vector<FieldNode*>& fields() const { return fieldsvec_;}
  bool HasField(const std::string& fieldname) const;
  FieldNode* GetField(const std::string& filedname) const;
 protected:
  StructDeclNode(const std::string& source, const SourceLoc& loc, ASTNodeType type)
      : ASTNode(source, loc, type) {}
 private:
  bool HasDuplication(const std::string& name);
  std::map<std::string, FieldNode*> fields_;
  std::vector<FieldNode*> fieldsvec_;
  std::string struct_name_;
  bool exchange_;
  DISALLOW_COPY_AND_ASSIGN(StructDeclNode);
};

class StructExternDeclNode : public StructDeclNode {
 public:
  StructExternDeclNode(const std::string& source, const SourceLoc& loc)
      : StructDeclNode(source, loc, kStructExternDeclNode)
      , decl_node_(NULL) {
  }
  virtual StructExternDeclNode* ToStructExternDeclNode() OVERRIDE {return this;}
  void SetDeclNode(StructDeclNode* node) {decl_node_ = node;}
  StructDeclNode* GetDeclNode() { return decl_node_;}
 private:
  StructDeclNode* decl_node_;
  DISALLOW_COPY_AND_ASSIGN(StructExternDeclNode);
};

class SwitchNode : public ASTNode {
  public:
  SwitchNode(const std::string& source, const SourceLoc& loc)
      : ASTNode(source, loc, kSwitchNode)
      , express_node_(NULL) {
  }
  virtual SwitchNode* ToSwitchNode() OVERRIDE { return this;}
  virtual bool IsSwitchNode() OVERRIDE { return true;}

  void SetExpressionNode(ASTNode* node) { express_node_ = node;}
  void SetStatements(ASTNode* node) { AddChildren(node);}
 private:
  ASTNode* express_node_;
  DISALLOW_COPY_AND_ASSIGN(SwitchNode);
};

class SymbolNode : public ASTNode {
 public:
  SymbolNode(const std::string& source, const SourceLoc& loc);
  virtual SymbolNode* ToSymbolNode() OVERRIDE { return this;}
  virtual bool IsSymbolNode() OVERRIDE { return true;}

  // lookup from
  void SetDeclarationNode(ASTNode* node);
  void SetInitializer(ASTNode* node);
  void SetSymbolName(const std::string& name);
  void SetTypedNode(ASTNode* node);

  ASTNode* GetInitializer() { return initializer_;}
  DeclarationNode* GetDeclarationNode() { return declared_;}
  TypedNode* GetTypedNode() { return typed_node_;}

  // is function's parameters
  bool IsFuncParamSymbol() const { return declared_ == NULL;}
  const std::string& symbolname();
  TypePtr& GetType();
  const TypePtr& GetType() const;
 private:
  std::string symbol_name_;
  TypedNode* typed_node_;
  DeclarationNode* declared_;
  ASTNode* initializer_;
  DISALLOW_COPY_AND_ASSIGN(SymbolNode);
};

/**
 * 当符号声明是具有的类型有此函数记录
 * 此外, 如果是 struct 类型，此函数还将记录结构体声明的节点
 * 如果是数组表达式此函数则记录数组表达式的节点
 */
class TypedNode : public ASTNode {
 public:
  TypedNode(const std::string& source, const SourceLoc& loc);
  virtual TypedNode* ToTypedNode() OVERRIDE {return this;}
  virtual bool IsTypedNode() OVERRIDE {return true;}

  void SetBasicType(BasicType type);
  void SetStructType(const std::string& name);
  void SetAnonymouseStruct(StructDeclNode* decl);
  void SetType(TypePtr type);
  TypePtr& GetType() { return type_;}
  const TypePtr& GetType() const { return type_;}

  StructDeclNode* GetStructDecl() { return decl_node_;}
  void SetStructDecl(ASTNode* node);
  bool IsArrayTyped() const { return dims_.size() > 0u;}
  const ASTNodeVec& GetDim() const { return dims_;}
  void AddDim(ASTNode* dim) {
    AddChildren(dim);
    dims_.push_back(dim);
  }
 private:
  TypePtr type_;
  StructDeclNode* decl_node_;
  ASTNodeVec dims_;
  DISALLOW_COPY_AND_ASSIGN(TypedNode);
};

/**
 * 单目运算符操作
 */
class UnaryOpNode : public ExpressionNode {
 public:
  UnaryOpNode(const std::string& source, const SourceLoc& loc)
      : ExpressionNode(source, loc, kUnaryOpNode)
      , op_(kOpNull)
      , oper_(NULL) {
  }
  virtual UnaryOpNode* ToUnaryOpNode() OVERRIDE {return this;}
  virtual bool IsUnaryOpNode() OVERRIDE {return true;}

  void SetOper(ASTNode* oper);
  void SetOperator(Operator o);

  ASTNode* GetOper() { return oper_;}
  Operator GetOperator() const { return op_;}
 private:
  Operator op_;
  ASTNode* oper_;
  DISALLOW_COPY_AND_ASSIGN(UnaryOpNode);
};

class WhileLoopNode : public ASTNode {
 public:
  WhileLoopNode(const std::string& source, const SourceLoc& loc);

  virtual WhileLoopNode* ToWhileLoopNode() OVERRIDE { return this;}
  virtual bool IsWhileLoopNode() OVERRIDE { return true;}
  
  void SetDoWhileStyle(bool do_while) { do_while_style_ = do_while;}
  void SetExprNode(ASTNode* node);
  void SetStatements(ASTNode* node);

  bool IsDoWhileLoop() const { return do_while_style_;}
  ASTNode* GetExprNode() { return expr_node_;}
  ASTNode* GetStatements() { return statements_;}
 private:
  bool do_while_style_;
  ASTNode* expr_node_;
  ASTNode* statements_;
  DISALLOW_COPY_AND_ASSIGN(WhileLoopNode);
};

class ASTNodeFactory {
 public:
  virtual ASTNode* Create(ASTNode::ASTNodeType type, const std::string& source,
                          const SourceLoc& loc);
};
}  // namespace afx
}  // namespace azer
