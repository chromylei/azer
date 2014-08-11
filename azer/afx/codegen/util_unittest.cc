#include "azer/afx/codegen/util.h"
#include "base/logging.h"
#include "gtest/gtest.h"

using azer::afx::FormatCode;


TEST(FormatCode, Structure) {
  const char* code = "struct VsOutput {"
      "vec4 position;"
      "vec4 normal;"
      "};";
  const char* expect = "struct VsOutput {\n"
      "  vec4 position;\n"
      "  vec4 normal;\n"
      "};\n";
  std::string res = FormatCode(code);
  ASSERT_STREQ(expect, res.c_str());
}
