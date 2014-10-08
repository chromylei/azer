#pragma once

#include "base/basictypes.h"
#include "azer/base/tree_node.h"

namespace azer {
namespace afx {

class ASTNode;
class DeclarationNode;
class FieldNode;
class FuncCallNode;
class RefSymbolNode;
class SymbolNode;
class ScopedNode;
class TypedNode;
class ParseContext;

/**
 * 此类的功能是变量整个语法树并将类型，结构体，变量的初始化
 * 与使用相对应
 * 严格说来 ContextLookup 也是 SyntaxValidator 的也部分，
 * 之所以将这个过程独立出来，不在 SyntaxValidator 的过程中顺便完成
 * 是由于在 SyntaxValidator 过程中可能会用到这些原始信息，如果这些原始信息
 * 不存在，会给后续流程计算带来麻烦，使流程看起来比较复杂，因此将其独立出来
 */
class ContextValidator {
 public:
  ContextValidator(ParseContext* context)
      : context_(context)
      , failed_(false) {
  }
  bool Valid(ASTNode* node);
  bool failed() const { return failed_;}
 private:
  void ReportError(ASTNode* node, const std::string& msg);
  void ReportError(ASTNode* node, const std::string& msg, int err);

  bool LookupSymbolDecl(RefSymbolNode* node);
  bool LookupFunctionDecl(FuncCallNode* node);
  bool LookupTypeDecl(ASTNode* node, TypedNode* typed);
  bool LookupField(ASTNode* node, FieldNode* field);
  bool AddSymbolToScoped(SymbolNode* node);
  bool CheckRefTexture(RefSymbolNode* reftype);
  bool CheckSymbolTexture(SymbolNode* reftype);
  /**
   * uniform 类型的变量声明时，只能有一个变量，不能是多个
   */
  bool ValidUniformDeclarationNode(DeclarationNode* decl);
  ScopedNode* GetScopedNode(ASTNode* node);
  ParseContext* context_;
  bool failed_;
  DISALLOW_COPY_AND_ASSIGN(ContextValidator);
};
}  // namespace afx
}  // namespace azer
