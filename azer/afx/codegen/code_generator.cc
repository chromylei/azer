
#include "azer/afx/codegen/code_generator.h"

#include "base/logging.h"
#include "azer/afx/compiler/astnode.h"

namespace azer {
namespace afx {

bool SnippetCodeGenerator::OnVisitBegin(ASTNode* node) {
  std::string str;
  ASTCodeGenerator* codegen = factory_->CreateCodeGen(node);
  node->set_extra(codegen);
  bool ret = codegen->GenCodeBegin(&str);
  code_gen_.append(str);
  return ret;
}

void SnippetCodeGenerator::OnVisitEnd(ASTNode* node) {
  std::string str;
  ASTCodeGenerator* codegen = (ASTCodeGenerator*)node->extra();
  DCHECK(codegen != NULL);
  codegen->GenCodeEnd(&str);
  code_gen_.append(str);
}

void SnippetCodeGenerator::GenCode(ASTNode* node) {
  code_gen_.clear();
  node->traverse(this);
}
} // namespace afx
} // namespace azer
