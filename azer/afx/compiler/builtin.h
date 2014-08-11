#pragma once

#include <string>
#include "base/basictypes.h"
#include "base/lazy_instance.h"

namespace azer {
namespace afx {

class ASTNode;
class Parser;
class ParseContext;
class ASTNodeFactory;

class BuiltinContext {
 public:
  BuiltinContext();
  virtual ~BuiltinContext();
  static ASTNode* Lookup(const std::string& name);
  static BuiltinContext* get();
 private:
  virtual void Init();
  ASTNode* LookupSymbol(const std::string& name);
  ParseContext* context_;
  ASTNodeFactory* factory_;

  friend struct ::base::DefaultLazyInstanceTraits<BuiltinContext>;
  DISALLOW_COPY_AND_ASSIGN(BuiltinContext);
};
}  // namespace afx
}  // namespace azer
