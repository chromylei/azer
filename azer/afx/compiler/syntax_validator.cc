#include "azer/afx/compiler/syntax_validator.h"

#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/context.h"
#include "base/strings/string_util.h"
#include "base/strings/string_piece.h"

namespace azer {
namespace afx {
bool SyntaxValidator::Refactor() {
  DCHECK(context_ && context_->root());
  if (!context_->success()) {
    return false;
  }

  context_->root()->traverse(this);
  return context_->success();
}

bool SyntaxValidator::OnVisitBegin(ASTNode* node) {
  return true;
}

void SyntaxValidator::OnVisitEnd(ASTNode* node) {
  if (!context_->success()) {
    return;
  }
  if (!context_validator_.Valid(node)) {
    return;
  }
  if (!express_validator_.Valid(node)) {
    return;
  }
}

}  // namespace afx
}  // namespace azer
