#pragma once

#include <iostream>
#include <string.h>
#include "base/build_info.h"
#include "base/logging.h"


namespace azer {
namespace afx {
// TODO(yanglei)
class NullStream {
};

inline const char* DebugRelativePath(const char* path) {
  DCHECK_EQ(strstr(path, ::base::kBuildRootDir), path);
  DCHECK_GT(strlen(path), ::base::kBuildRootDirLength + 1);
  return path + ::base::kBuildRootDirLength + 1;
}

inline std::string RelativePath(const std::string& path) {
  if (path.find(::base::kBuildRootDir) == 0) {
    DCHECK_GT(path.length(), ::base::kBuildRootDirLength + 1);
    return path.substr(::base::kBuildRootDirLength + 1);
  } else {
    return path;
  }
}
}  // namespace afx
}  // namespace azer

#define LEX_TRACE \
  ((azer::afx::ParseContext*)context)->tokenizer_stream() << "[Tokenizer] "
#define PARSER_TRACE \
  ((azer::afx::ParseContext*)context)->parser_stream()     \
  << azer::afx::DebugRelativePath(__FILE__)                \
  << ":" << __LINE__  << " [Parser] "

#define PRE_PARSER_TRACE \
  (parseContext)->parser_stream()                               \
  << azer::afx::DebugRelativePath(__FILE__)                \
  << ":" << __LINE__  << " [Parser] "
