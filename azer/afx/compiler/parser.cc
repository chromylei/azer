#include "azer/afx/compiler/parser.h"

#include "base/logging.h"
#include "lex.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/builtin.h"
#include "azer/afx/compiler/syntax_validator.h"

extern int afx_yyparse(void*);

namespace azer {
namespace afx {

Parser::Parser() {
}

// class Parse
bool Parser::Parse(ParseContext* context) {
  DCHECK(context->root() == NULL);
  DCHECK(context->success());
  SourceLoc loc; loc.lineno = 0; loc.index = 0;
  context->root_ = context->Create(ASTNode::kScopedNode, loc)->ToScopedNode();
  
  yy_scan_string(context->source().c_str());
  afx_yyparse((void*)context);

  if (context->success() && context->options().syntax_valid) {
    SyntaxValidator validator(context);
    validator.Refactor();
  }

  return context->success();
}
}  // namespace afx
}  // namespace azer
