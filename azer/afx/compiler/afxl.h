#pragma once

#include <string>
#include <ostream>
#include "base/files/file_path.h"

#define AFXL FILE_PATH_LITERAL

namespace azer {
namespace afx {
inline std::ostream& operator << (std::ostream& os, const std::wstring& str) {
  return os << str.c_str();
}
}  // namespace afx
}  // namespace azer
