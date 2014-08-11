#pragma once

#include "azer/afx/compiler/astdump.h"
#include "azer/afx/linker/linker.h"


inline void Dump(azer::afx::AfxLinker* linker) {
  std::string str;
  azer::afx::ASTreeDumper dumper;
  DCHECK(linker->root());
  dumper.Dump(linker->root()->GetContext()->root(), &str);
  LOG(ERROR) << str;
};

inline void DumpLinkError(azer::afx::AfxLinker* linker) {
  if (linker->success()) { return;}
  if (!linker->GetCompileError().empty()) {
    std::cerr << "compiler error: \n" << linker->GetCompileError() << std::endl;
  }
  if (!linker->GetErrorText().empty()) {
    std::cerr << "link error: \n" << linker->GetErrorText() << std::endl;
  }
}
