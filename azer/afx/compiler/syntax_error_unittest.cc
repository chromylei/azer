#include "gtest/gtest.h"
#include "azer/afx/compiler/astdump.h"
#include "azer/afx/compiler/parser.h"
#include "azer/afx/compiler/testutil.h"

using azer::afx::Parser;
using azer::afx::ParseContext;
using azer::afx::SyntaxValidator;
using azer::afx::ASTNodeFactory;

TEST(SyntaxError, DeclaredSymbol) {
  const std::string str = 
    "void func() {\n"
    "  int a = 0; \n"
    "  mat4 a;\n"
    "}\n";
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory);
  Parser parser;
  EXPECT_FALSE(parser.Parse(&context));
  // dump_error(&context, std::cout);
}

TEST(SyntaxError, ArrayInitWithVariable) {
  const std::string str = 
      "void func() {\n"
      " int size = 10;\n"
      " int b[size * 10 + 1 - 5 * 8 / 2];\n"
      "}\n";
  azer::afx::ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_FALSE(parser.Parse(&context));
  // dump_error(&context, std::cout);
}

TEST(SyntaxError, ArrayWithoutDim) {
  std::string cases[] = {
    "void func() {int arr[];}\n",
    "void func() {int arr[1][];}\n",
  };

  for (int i = 0; i < (int)arraysize(cases); ++i) {
    azer::afx::ParseContext::Options opt;
    // opt.dump_parser = true;
    // opt.dump_tokenizer = true;
    ASTNodeFactory factory;
    ParseContext context(AFXL(""), "", cases[i], &factory, opt);
    Parser parser;
    EXPECT_FALSE(parser.Parse(&context));
    EXPECT_EQ(context.GetErrno(), azer::afx::kArrayDimNotSpecified);
    // DumpASTree(&context, std::cout);
    // dump_error(&context, std::cout);
  }
}

TEST(SyntaxError, ArrayNotConstent) {
  std::string cases[] = {
    "void func() { int arr[2] = {1};}",
    "void func() { int arr[][2] = {{1, 2}, {2, 3, 1}};}",
  };

  azer::afx::CompileErrno error_num[] = {
    azer::afx::kArrayDimNotConsistent,
    azer::afx::kArrayDimNotConsistent,
  };

  for (int i = 0; i < (int)arraysize(cases); ++i) {
    azer::afx::ParseContext::Options opt;
    // opt.dump_parser = true;
    // opt.dump_tokenizer = true;
    ASTNodeFactory factory;
    ParseContext context(AFXL(""), "", cases[i], &factory, opt);
    Parser parser;
    EXPECT_FALSE(parser.Parse(&context));
    EXPECT_EQ(context.GetErrno(), error_num[i]);
    // DumpASTree(&context, std::cout);
    // dump_error(&context, std::cout);
  }
}

TEST(SyntaxError, MultiUniformVariable) {
  const std::string str = "uniform mat4 mvp, world;\n";
  azer::afx::ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_FALSE(parser.Parse(&context));
  EXPECT_EQ(context.GetErrno(), azer::afx::kMultiUniformDeclInStatement);
  // DumpASTree(&context, std::cout);
  // dump_error(&context, std::cout);
}

TEST(SyntaxError, StructDeclMultiTimes) {
  std::string cases[] = {
    "struct Light1 {\n"
    "  vec3 v1;\n"
    "};\n"
    "struct Light1 {\n"
    "  vec3 v1;\n"
    "};\n",
  };

  /**
   * 注意的是，此处报的错误不是 “struct 重复定义” 类的错误
   * 这是有与在同名的 struct 定义以后，在 tokenizer 阶段返回的将不再是 IDENTIFIER
   * 而是 TYPE_NAME, 此时没有能够与它匹配的语法规则, 因此它报的会是一个语法错误
   */
  for (int i = 0; i < (int)arraysize(cases); ++i) {
    azer::afx::ParseContext::Options opt;
    // opt.dump_parser = true;
    // opt.dump_tokenizer = true;
    ASTNodeFactory factory;
    ParseContext context(AFXL(""), "", cases[i], &factory, opt);
    Parser parser;
    EXPECT_FALSE(parser.Parse(&context));
    EXPECT_EQ(context.GetErrno(), azer::afx::kSyntaxError);
    // DumpASTree(&context, std::cout);
    // dump_error(&context, std::cout);
  }
}

TEST(SyntaxError, FuncDefMultiTimes) {
  std::string cases[] = {
    "void func1() {}\n"
    "void func1() {}\n"
  };

  for (int i = 0; i < (int)arraysize(cases); ++i) {
    azer::afx::ParseContext::Options opt;
    // opt.dump_parser = true;
    // opt.dump_tokenizer = true;
    ASTNodeFactory factory;
    ParseContext context(AFXL(""), "", cases[i], &factory, opt);
    Parser parser;
    EXPECT_FALSE(parser.Parse(&context));
    EXPECT_EQ(context.GetErrno(), azer::afx::kFuncDefined);
    // DumpASTree(&context, std::cout);
    // dump_error(&context, std::cout);
  }
}

TEST(SyntaxError, TextureMustBeUniform) {
  std::string cases[] = {
    "Texture2D tex[2];\n"
    "vec4 func1() {\n"
    "  return sampler2D(tex, vec(0, 0));\n"
    "}\n",
    "vec4 func1() {\n"
    "Texture2D tex[2];\n"
    "  return sampler2D(tex, vec(0, 0));\n"
    "}\n"
  };

  for (int i = 0; i < (int)arraysize(cases); ++i) {
    azer::afx::ParseContext::Options opt;
    // opt.dump_parser = true;
    // opt.dump_tokenizer = true;
    ASTNodeFactory factory;
    ParseContext context(AFXL(""), "", cases[i], &factory, opt);
    Parser parser;
    EXPECT_FALSE(parser.Parse(&context));
    EXPECT_EQ(context.GetErrno(), azer::afx::kTextureMustbeUniformed);
    // DumpASTree(&context, std::cout);
    // dump_error(&context, std::cout);
  }
}


TEST(SyntaxError, FuncParamNotMatch) {
  std::string cases[] = {
    "uniform Texture2D tex[2];\n"
    "vec4 func1() {\n"
    "  return sampler2D(tex, vec(0, 0));\n"
    "}\n"
  };

  for (int i = 0; i < (int)arraysize(cases); ++i) {
    azer::afx::ParseContext::Options opt;
    // opt.dump_parser = true;
    // opt.dump_tokenizer = true;
    ASTNodeFactory factory;
    ParseContext context(AFXL(""), "", cases[i], &factory, opt);
    Parser parser;
    EXPECT_FALSE(parser.Parse(&context));
    EXPECT_EQ(context.GetErrno(), azer::afx::kFuncDefined);
    // DumpASTree(&context, std::cout);
    // dump_error(&context, std::cout);
  }
}
