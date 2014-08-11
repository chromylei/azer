#pragma once

#include "azer/base/tree_node.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/errno.h"
#include "azer/afx/compiler/context_validator.h"
#include "azer/afx/compiler/expression_validator.h"

namespace azer {
namespace afx {
class ParseContext;

class SyntaxValidator : public TreeNode<ASTNode>::Traverser {
 public:
  SyntaxValidator(ParseContext* context)
    : context_(context)
    , context_validator_(context)
    , express_validator_(context) {
  }
  bool Refactor();
 private:
  virtual bool OnVisitBegin(ASTNode* node) OVERRIDE;
  virtual void OnVisitEnd(ASTNode* node) OVERRIDE;

  ParseContext* context_;
  ContextValidator context_validator_;
  ExpressionValidator express_validator_;
  DISALLOW_COPY_AND_ASSIGN(SyntaxValidator);
};

}  // namespace afx
}  // namespace azer
