#pragma once

#include <set>
#include <string>
#include <vector>

#include "azer/base/tree_node.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/linker/technique_parser.h"
#include "base/basictypes.h"

namespace azer {
namespace afx {
/**
 * class DependentCalc
 * calculate the dependency
 */
class DependCalactor : public TreeNode<ASTNode>::Traverser {
 public:
  DependCalactor() {}

  /**
   * 计算依赖的基本思路
   * 首先计算根节点的依赖项（注意此处的根节点，并非 ASTNode Tree 的根节点，
   * 而是计算依赖的起始节点），依赖项将被放置在 depend_ 的 vector 当中
   * 接下来遍历这个向量并计算依赖，这个过程中“依赖向量“是在不断增长的。
   * 当不存在更多依赖项，增长将停止，保存下来的就是全部依赖项了
   */
  void Calc(ASTNode* node);
  const ASTNodeVec& GetDepend() const { return depend_;}
  void Swap(TechniqueParser::StageInfo* tech);
 protected:
  void CalcRecursive(ASTNode* node);
  bool OnVisitBegin(ASTNode* node) OVERRIDE;
  virtual void OnVisitEnd(ASTNode* node) OVERRIDE {}
 private:
  void CheckUniformTexture(ASTNode* node);
  void AddTexture(ASTNode* node);
  void AddFuncDep(ASTNode* node);
  void AddStructDeclDep(ASTNode* node);
  void AddSymbolNodeDep(ASTNode* node);
  void AddFieldNodeDep(ASTNode* node);
  void AddRefSymbolNode(ASTNode* node);
  void AddTypeDep(TypePtr type, ASTNode* node);
  void AddUniTypeDep(TypePtr type, ASTNode* node);
  bool Add(const std::string& name, ASTNode* node);
  std::set<std::string> inserted_;
  std::set<std::string> tex_inserted_;
  std::set<std::string> uni_dep_inserted_;
  ASTNodeVec depend_;
  ASTNodeVec uniforms_;
  ASTNodeVec textures_;
  ASTNodeVec uni_depend_;
  DISALLOW_COPY_AND_ASSIGN(DependCalactor);
};
}  // namespace afx
}  // namespace azer
