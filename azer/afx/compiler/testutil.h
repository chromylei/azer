#pragma once

#include <iostream>
#include <string>
#include "azer/afx/compiler/context.h"
#include "azer/afx/compiler/afxl.h"
#include "base/files/file_path.h"

inline void dump_error(azer::afx::ParseContext* context, std::ostream& os) {
  for (auto iter = context->GetErrorTextVec().begin();
       iter != context->GetErrorTextVec().end(); ++iter) {
    os << *iter << std::endl;
  }
}

#define DUMP_AFXCOMPILE_ERROR(context)             \
  if (!((context).success())) {                    \
    dump_error(&(context), std::cout);             \
    return;                                        \
  }

inline std::string DumpASTree(azer::afx::ParseContext* context, std::ostream& os) {
  azer::afx::ASTreeDumper dumper;
  std::string dump;
  dumper.Dump(context->root(), &dump);
  os << dump << std::endl;
  return dump;
}
