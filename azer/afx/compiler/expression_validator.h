#pragma once

#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/errno.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/base/tree_node.h"
#include "base/basictypes.h"

namespace azer {
namespace afx {

/**
 * 计算常量表达式的值
 * 目前仅仅支持基本类型计算 +, -, *, / 等等
 */
class ConstExpressValueCalculator : public TreeNode<ASTNode>::Traverser {
 public:
  ConstExpressValueCalculator();
  ~ConstExpressValueCalculator();
 private:
  BasicType basic_type_;
  union {
    bool b;
    char c;
    short s;
    uint32 u32;
    int32 i32;
    float f;
    double d;
  } value_;

  virtual bool OnVisitBegin(ASTNode* node) OVERRIDE;
  virtual void OnVisitEnd(ASTNode* node) OVERRIDE;
  DISALLOW_COPY_AND_ASSIGN(ConstExpressValueCalculator);
};

/**
 * 计算运算符是否有效，以及运算符的结果
 */
class ExpressionValidator {
 public:
  ExpressionValidator(ParseContext* context)
      : context_(context)
      , failed_(false) {
  }

  bool Valid(ASTNode* node);
 private:
  void ReportError(const std::string& errdesc, ASTNode* node,
                   CompileErrno err = kSyntaxError);
  bool HasError() const  { return failed_;}
  
  void CalcUnaryOperType(UnaryOpNode* node);
  void CalcBinaryOperType(BinaryOpNode* node);

  // check if the expression is constant value or not
  /**
   * 通过 initializer 制定或者直接通过 decl 直接指定常量
   */
  bool CheckArrayDecl(SymbolNode* node);
  bool CheckConstantExpression(ASTNode* node);
  bool ValidSwizzleExpression(const TypePtr& type, FieldNode* field);
  bool CalcArrayDim(ASTNode* node);
  bool ValidArrayDimSpecified(TypedNode* typed);
  bool CheckInitializer(ASTNode* node);
  bool InitFieldNodeType(ASTNode* node);
  bool InitParamNodeType(ASTNode* node);
  int CalcInitializerDim(ASTNode* node, ASTNodeVec* vec);
  int CalcInitializerLevelDim(const ASTNodeVec& node, ASTNodeVec* vec);

  /**
   * MaxType 仅仅计算那个类型的表达能力更强，而不进行合理性计算
   * 合理性计算有 valid*** 相关函数完成
   */
  BasicType MaxType(TypePtr type1, TypePtr type2);
  TypePtr GetMulOperType(ASTNode* node1, ASTNode* node2);
  bool ValidMulOperType(ASTNode* node1, ASTNode* node2);
  ParseContext* context_;
  bool failed_;
  DISALLOW_COPY_AND_ASSIGN(ExpressionValidator);
};

/**
 * 获得指定节点的类型（不进行任何计算）
 */
TypePtr GetNodeType(ASTNode* node);
TypedNode* GetTypedNode(ASTNode* node);
}  // namespace afx
}  // namespace azer
