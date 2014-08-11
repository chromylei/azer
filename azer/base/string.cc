#include "azer/base/string.h"
#include "base/strings/utf_string_conversions.h"

namespace azer {
std::ostream& operator << (std::ostream& os, const wchar_t* str) {
  os << ::base::WideToUTF8(str);
  return os;
}

}  // namespace azer
