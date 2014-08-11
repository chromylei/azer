#pragma once

#include "azer/afx/compiler/astnode.h"

namespace azer {
namespace afx {
class ASTreeDumper : public TreeNode<ASTNode>::Traverser {
 public:
  ASTreeDumper() : depth_(0) {
  }

  void Dump(ASTNode* node, std::string* str);
 private:
  int depth() const { return depth_;}
  virtual bool OnVisitBegin(ASTNode* node) OVERRIDE;
  virtual void OnVisitEnd(ASTNode* node) OVERRIDE;
  std::string dump_str_;
  int depth_;
  DISALLOW_COPY_AND_ASSIGN(ASTreeDumper);
};
}  // namespace afx
}  // namespace azer
