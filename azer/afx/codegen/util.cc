#include "azer/afx/codegen/util.h"

#include "azer/afx/compiler/astnode.h"
#include "base/strings/string_util.h"
#include "base/strings/string_split.h"
#include "base/file_util.h"
#include <sstream>

namespace azer {
namespace afx {
std::string FormatCode(const std::string& code) {
  std::stringstream out;
  int depth = 0;
  bool prev_new_line = false;
  const char* cur = code.c_str();
  while (*cur != '\0') {
    if (StartsWithASCII(cur, "};", true)) {
      out << "};\n";
      prev_new_line = true;
      cur += 2;
      depth--;
    } else if (StartsWithASCII(cur, ";\n", true)) {
      out << ";\n";
      prev_new_line = true;
      cur += 2;
    } else if (*cur == '}') {
      out << "}\n";
      prev_new_line = true;
      cur++;
      depth--;
    } else if (*cur == '{') {
      out << "{\n";
      depth++;
      prev_new_line = true;
      cur++;
    } else if (*cur == ';') {
      out << ";\n";
      prev_new_line = true;
      cur++;
    } else {
      if (prev_new_line) {
        out << std::string(depth * 2, ' ');
        prev_new_line = false;
      }
      out << *cur;
      cur++;
    }
  }

  return out.str();
}
}  // namespace afx
}  // namespace azer

bool CompareCode(const ::base::FilePath& path, const std::string& content) {
  std::string content1;
  if (!::base::ReadFileToString(path, &content1)) {
    return false;
  }

  std::string str1, str2;
  ::base::ReplaceChars(content1, "\r\n", "", &str1);
  ::base::ReplaceChars(content, "\r\n", "", &str2);
  if (str1 == str2) {
    return true;
  } else {
    LOG(ERROR) << "EXPECT:\n" << str1;
    LOG(ERROR) << "RESULT:\n" << str2;
    return false;
  }  
}
