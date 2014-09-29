#include "azer/base/string.h"
#include "base/strings/utf_string_conversions.h"

namespace azer {
std::ostream& operator << (std::ostream& os, const wchar_t* str) {
  std::string s = std::move(::base::WideToUTF8(str));
  os << s;
  return os;
}

}  // namespace azer
