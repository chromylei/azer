#include "azer/afx/linker/program.h"

#include "base/logging.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/parser.h"
#include "azer/afx/preprocessor/parser.h"

namespace azer {
namespace afx {

bool Program::Preprocess(PreprocessorParser::Delegate* delegate)  {
  DCHECK(preprocessor_.get() == NULL);
  DCHECK(context_.get() == NULL);
  PreprocessorParser* pparser = parent() ? parent()->preprocessor_.get(): NULL;
  preprocessor_.reset(new PreprocessorParser(pparser, delegate, false));
  preprocessor_->set_extra(this);
  if (!preprocessor_->Parse(code_)) {
    return false;
  }

  // parse code
  const std::string& code = preprocessor_->GetCode();
  context_.reset(new ParseContext(path_, preprocessor_->package(),
                                  code, &factory_));
  return true;
}

bool Program::ParseASTree(Program* parent) {
  DCHECK(preprocessor_.get() != NULL);
  DCHECK(context_.get() != NULL);
  if (parent) {
    parent->context_->AddChild(context_.get());
  }

  Parser parser;
  if (!parser.Parse(context_.get())) {
    return false;
  }
  return context_->success();
}

ASTNode* Program::LookupSymbol(const std::string& symbol) {
  DCHECK(preprocessor_.get() != NULL);
  DCHECK(context_.get() != NULL);
  return NULL;
}

std::string Program::GenCode() {
  std::stringstream ss;

  return ss.str();
}
}  // namespace afx
}  // namespace azer

