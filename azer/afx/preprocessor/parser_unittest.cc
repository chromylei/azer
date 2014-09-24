#include "azer/afx/preprocessor/parser.h"
#include "azer/afx/compiler/afxl.h"

#include "preprocessor_tab.h"
#include "gtest/gtest.h"

using azer::afx::PreprocessorParser;

TEST(PreprocessorParser, Pckagename) {
  const std::string code = ""
      "#package base::light";
      ;
  PreprocessorParser parser(NULL, false);
  ASSERT_TRUE(parser.Parse(code));
  ASSERT_EQ(parser.package(), "base::light");
}

TEST(PreprocessorParser, MultiPackage) {
  const std::string code = ""
      "#package base::light\n"
      "#package base::light";
      ;
  PreprocessorParser parser(NULL, false);;
  ASSERT_FALSE(parser.Parse(code));
}

TEST(PreprocessorParser, Include) {
  const std::string code = ""
      "#include \"afx/base.afxi\"\n"
      "#include \"afx/light.afxi\"\n"
      "#include \"afx/material.afxi\"\n"
      ;
  PreprocessorParser parser(NULL, false);;
  ASSERT_TRUE(parser.Parse(code));
  ASSERT_EQ(parser.includes().size(), 3u);
  ASSERT_EQ(parser.includes()[0], AFXL("afx//base.afxi"));
  ASSERT_EQ(parser.includes()[1], AFXL("afx//light.afxi"));
  ASSERT_EQ(parser.includes()[2], AFXL("afx//material.afxi"));
  ASSERT_TRUE(parser.package().empty());
}

TEST(PreprocessorParser, Define) {
  const std::string code = ""
      "#define AFX_VERSION 3000UL\n"
      "#define WIN_VERSION 2000UL\n"
      "#define _WIN_\n"
      "#undef _WIN_\n"
      "#define _LINUX_\n"
      ;
  PreprocessorParser parser(NULL, false);;
  ASSERT_TRUE(parser.Parse(code));
  ASSERT_EQ(parser.GetDefinedValue("AFX_VERSION"), "3000UL");
  ASSERT_EQ(parser.GetDefinedValue("WIN_VERSION"), "2000UL");
  ASSERT_FALSE(parser.IsDefined("_WIN_"));
  ASSERT_TRUE(parser.IsDefined("_LINUX_"));
}

/*
TEST(PreprocessorParser, Ifdef) {
  const std::string code = ""
      "#define WIN\n"
      "#ifdef WIN\n"
      "# define AFX_VERSION 3000UL\n"
      "#endif\n"
      "#if defined(_WIN_)\n"
      "# define _MSVS_\n"
      "#else\n"
      "# define _MSVC_\n"
      "#endif\n"
      "#if !defined(_LINUX_)\n"
      "# define _GCC_\n"
      "#endif\n"
      ;
  PreprocessorParser parser(NULL, true);;
  ASSERT_TRUE(parser.Parse(code));
  ASSERT_EQ(parser.GetDefinedValue("AFX_VERSION"), "3000UL");
  ASSERT_FALSE(parser.IsDefined("_WIN_"));
  ASSERT_TRUE(parser.IsDefined("_GCC_"));
}


TEST(PreprocessorParser, ifelse) {
  const std::string code = ""
      "#define WIN98\n"
      "#ifdef WIN\n"
      "# define VERSION WIN98\n"
      "#elif defined(_WINNT_)\n"
      "# define VERSION WINNT\n"
      "#elif defined(_WINXP_)\n"
      "# define VERSION WINXP\n"
      "#elif defined(_WIN7_)\n"
      "# define VERSION WIN7\n"
      "#else\n"
      "# define VERSION UNKNOWN\n"
      "#endif\n"
      ;
  PreprocessorParser parser(NULL, true);;
  ASSERT_TRUE(parser.Parse(code));
  ASSERT_EQ(parser.GetDefinedValue("AFX_VERSION"), "3000UL");
  ASSERT_FALSE(parser.IsDefined("_WIN_"));
  ASSERT_TRUE(parser.IsDefined("_GCC_"));
}
*/
