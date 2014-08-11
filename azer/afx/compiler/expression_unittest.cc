#include "gtest/gtest.h"
#include "azer/afx/compiler/astdump.h"
#include "azer/afx/compiler/parser.h"
#include "azer/afx/compiler/testutil.h"

using azer::afx::Parser;
using azer::afx::ParseContext;
using azer::afx::ASTNodeFactory;

TEST(Expression, Correct_Mul) {
  const std::string str = 
      "mat4 func(in mat4 b) {    \n"
      " vec3 v1 = vec3(1, 1, 1); \n"
      " mat3 m1;                 \n"
      " vec3 v2 = v1 * m1;       \n"
      " vec4 v3 = vec4(1, 1, 1, 1); \n"
      " mat4 m2;                 \n"
      " vec4 v4 = v3 * m2;       \n"
      " return m2;               \n"
      "}\n";
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  dump_error(&context, std::cout);
}

TEST(ExpressionError, Mul) {
  const std::string cases[] = {
    "vec3 func(in mat4 b) {   \n"
    " vec3 a = vec3(1, 1, 1); \n"
    " return a * b;           \n"
    "}\n",
    /* mat4 * vec3*/
    "vec4 func(in mat4 b) {   \n"
    " vec3 a = vec3(1, 1, 1); \n"
    " mat4 m;                 \n"
    " return a * m;           \n"
    "}\n",
    /* vec3 * mat4 */
    "vec3 func(in mat4 b) {   \n"
    " vec3 a = vec3(1, 1, 1); \n"
    " mat4 m;                 \n"
    " return m * a;           \n"
    "}\n",
  };

  for (size_t i = 0; i < arraysize(cases); ++i) {
    ASTNodeFactory factory;
    ParseContext context("", "", cases[i], &factory);
    Parser parser;
    ASSERT_FALSE(parser.Parse(&context));
  }
}

TEST(ExpressionError, ReturnTypeNotMatched) {
  ASSERT_TRUE(false);
}

TEST(ExpressionError, StructHasNoField) {
  const std::string cases[] = {
    "struct Light1 {\n"
    "  vec3 v1;\n"
    "};\n"
    "float mydoc(Light1 l1, vec3 v2) {\n"
    "  return dot(l1.v2, v2);\n"
    "}\n",
  };

  for (size_t i = 0; i < arraysize(cases); ++i) {
    ASTNodeFactory factory;
    ParseContext context("", "", cases[i], &factory);
    Parser parser;
    ASSERT_FALSE(parser.Parse(&context));
  }
}

TEST(ExpressionError, VectorSwizze) {
  const std::string cases[] = {
    "void func(in vec4 v) {   \n"
    "  vec3 = v.rgbx;         \n"
    "}\n",
    "void func(in vec4 v) {   \n"
    "  vec3 = v.rgba;         \n"
    "}\n",
    "void func(in vec3 v) {   \n"
    "  vec3 = v.aaa;         \n"
    "}\n",
  };

  for (size_t i = 0; i < arraysize(cases); ++i) {
    ASTNodeFactory factory;
    ParseContext context("", "", cases[i], &factory);
    Parser parser;
    ASSERT_FALSE(parser.Parse(&context));
  }
}

TEST(ExpressionError, FunctionParamTypeNotMatch) {
  /**
   * 由于查找函数过程中实际上使用了参数，如果查找不到参数
   * 则意味的函数的参数不匹配
   * 其中有问题的是 TypeInit, 他的实现目前无法进行语法检查
   * 此外，FuncCall 的 built-in 函数需要进行大量的 overload
   */
  ASSERT_TRUE(true);
}

