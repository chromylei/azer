#pragma once

#include <string>
#include "base/basictypes.h"
#include "azer/afx/compiler/context.h"

namespace azer {
namespace afx {

class Parser {
 public:
  Parser();

  bool Parse(ParseContext* context);
 private:
  void RefactASTree(ParseContext* context);
  DISALLOW_COPY_AND_ASSIGN(Parser);
};

}  // namespace afx
}  // namespace azer
