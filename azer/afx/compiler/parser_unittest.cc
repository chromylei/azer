#include "gtest/gtest.h"
#include "azer/afx/compiler/astdump.h"
#include "azer/afx/compiler/parser.h"
#include "azer/afx/compiler/testutil.h"

using azer::afx::Parser;
using azer::afx::ParseContext;
using azer::afx::ASTNodeFactory;

TEST(AfxParser, Expression) {
  const std::string str =
      "void psmain() {\n"
      "  int a = 0;//comments\n"
      "  100;a++;--a;-a;a++++++;++++++a;++-+a; //\n"
      "  float f1 = 1.0f;\n"
      "  float f2 = 1.0f + 100.0f;\n"
      "  }  // comments"
      " // comments"
      ;
  ASTNodeFactory factory;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  // opt.syntax_valid = false;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  // DumpASTree(&context, std::cout);
  DUMP_AFXCOMPILE_ERROR(context);
}

TEST(AfxParser, FuncExpression) {
  const std::string str =
      "vec4 clamp(vec4 v, float v1, float v2) { return v;}"
      "void psmain() {\n"
      "  vec4 r1 = clamp(vec4(0.1, 0.1, 0.1, 1.0), 0.0f, 1.0f);"
      "  vec3 r2 = clamp(vec4(0.1, 0.1, 0.1, 1.0), 0.0f, 1.0f).xyz;"
      "}";
      ;
  ASTNodeFactory factory;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  // opt.syntax_valid = false;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  // DumpASTree(&context, std::cout);
  DUMP_AFXCOMPILE_ERROR(context);
}

TEST(AfxParser, ExpressionValue) {
  const std::string str =
      "void psmain() {\n"
      "  int a = 0;\n"
      "  100;a++;--a;-a;a++++++;++++++a;++-+a;\n"
      "  vec4 v1, diffuse;\n"
      "  float scalar = 0.1f;\n"
      "  v1 += diffuse * scalar;\n"
      "  vec4 v2 = -v1;\n"
      "}"
      ;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  // DumpASTree(&context, std::cout);
  DUMP_AFXCOMPILE_ERROR(context);
}

TEST(AfxParser, ExternalSymbol) {
  const std::string light_str = ""
      "struct SpotLight {\n"
      "  vec3 position;\n"
      "};\n";
  const std::string str =
      "Light::SpotLight light;\n"
      "void psmain() {        \n"
      "}                      \n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext lightctx(AFXL(""), "Light", light_str, &factory, opt);
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&lightctx));

  context.AddChild(&lightctx);
  EXPECT_TRUE(parser.Parse(&context));
  // DumpASTree(&context, std::cout);
  DUMP_AFXCOMPILE_ERROR(context);
}

TEST(AfxParser, VariableDeclaration) {
  std::string str =
      "void psmain() {                                         \n"
      "  bool b = false;                                       \n"
      "  char c = 'a';                                         \n"
      "  int i;                                                \n"
      "  const int i2 = 100;                                   \n"
      "  int ia, ib = 2, ic=3, id;                             \n"
      "  uint u = 10u;                                         \n"
      "  uniform vec3 v1;                                      \n"
      "  in uniform vec3 v2;                                   \n"
      "  mat4 m1;                                              \n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  DUMP_AFXCOMPILE_ERROR(context);
}

TEST(AfxParser, OperatorValidator) {
  
  std::string expr_case[] = {
    "void psmain() {\n"
    "  float factor = 1.0f;\n"
    "  vec3 v1, v2;\n"
    "  v2 = v1 * factor;\n"
    "  v2 = factor * v2;\n"
    "}",
    "void psmain() {\n"
    "  float factor = 1.0f;\n"
    "  vec3 v1, v2;\n"
    "  vec4 r = vec4(v1 * factor + v2.xyz, 1.0);\n"
    "}",
  };

  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  for (size_t i = 0; i < arraysize(expr_case); ++i) {
    ASTNodeFactory factory;
    ParseContext context(AFXL(""), "", expr_case[i], &factory, opt);
    Parser parser;
    EXPECT_TRUE(parser.Parse(&context)) << "failed at case: " << i;
    DUMP_AFXCOMPILE_ERROR(context);
  }
}

TEST(AfxParser, ArrayVariableDeclaration) {
  std::string str =
      "void psmain() {                                         \n"
      "  int arr[][2] = {{1, 2}, {2, 3}};                      \n"
      "  float farr1[] = {0.1f};                               \n"
      "  float farr1_ref = farr1[0];                           \n"  
      "  float farr2[] = {0.1f, 0.3, 0.7};                     \n"
      "  float farr3[3][3][3];                                 \n"
      "  float farr4[][] = { {0.1, 0.2, 0.3}, {0.4, 0.5, 0.6}};\n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  // opt.syntax_valid = false;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  // DumpASTree(&context, std::cout);
  DUMP_AFXCOMPILE_ERROR(context);
}

TEST(AfxParser, AttributeDeclaration) {
  const std::string str = 
      "[uniform_func=\"SetWorld\" default=\"identity\"] uniform mat4 wvp;"
      ;

  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, AttributeFunction) {
  const std::string str =
      "[system=\"true\" semantic=\"color\"]"
      "vec4 psmain() {\n"
      "  return vec4(1, 1, 1, 1);\n"
      "}\n"
      ;

  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, StructureDeclaration) {
  const std::string str =
      "struct VSoutput1 {\n"
      "  vec3 position;\n"
      "  vec3 normal;\n"
      "  vec2 texcoord;\n"
      "};\n"
      "struct VSoutput2 {\n"
      "  vec3 position;\n"
      "  vec3 normal;\n"
      "  vec2 texcoord;\n"
      "} object1;\n"
      "struct VSoutput3 {\n"
      "  [semantic=\"position\"] "
      "  vec3 position;\n"
      "  vec3 normal;\n"
      "  vec2 texcoord;\n"
      "};"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  dump_error(&context, std::cout);
}

TEST(AfxParser, AttributeStructureDecl) {
  const std::string str =
      "[cppstruct=\"azer::afx::DirLight\" cpphead=\"azer/afx/base/light.afx.h\"]"
      "struct DirLight {\n"
      "  vec3 direction;\n"
      "  vec3 ambient;\n"
      "  vec3 diffuse;\n"
      "};\n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  DUMP_AFXCOMPILE_ERROR(context);
}

TEST(AfxParser, StructureMember) {
  const std::string str =
      "struct VSoutput1 {          \n"
      "  vec3 position;            \n"
      "  vec3 normal;              \n"
      "  vec2 texcoord[3];         \n"
      "};                          \n"
      "struct My {                 \n"
      "  VSoutput1 o;              \n"
      "  vec4 v;                   \n"
      "};                          \n"
      "void main() {               \n"
      "  My m;                     \n"
      "  vec3 v3 = m.o.position;   \n"
      "  vec2 v2 = m.o.texcoord[0];\n"
      "}                           \n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  opt.syntax_valid = false;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  // DumpASTree(&context, std::cout);
  DUMP_AFXCOMPILE_ERROR(context);
}

TEST(AfxParser, AnomoymousStructSymbol) {
  const std::string str =
      "struct {\n"
      "  vec3 position;\n"
      "  vec3 normal;\n"
      "  vec2 texcoord;\n"
      "} object2;\n"
      ;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, FunctionDeclaration) {
  const std::string str =
      "struct Light {vec4 position;};\n"
      "void clamp0();"
      "void clamp1(float a);"
      "void clamp2(float a, float b);"
      "void clamp3(float a, float b, float);"
      "vec4 clamp4(float a, float b, float);"
      "Light clamp5(float a, float b, float);"
      "struct Light clamp5(float a, float b, float);"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, Texture) {
  const std::string str =
      "uniform Texture1D samp1d;\n"
      "uniform Texture1DArray samp1darr;\n"
      "uniform Texture2D samp2d;\n"
      "uniform Texture2DArray samp2darr;\n"
      "uniform Texture3D samp3d;\n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, TextureAsParam) {
  const std::string str =
      "vec4 Sample(Texture2D tex, vec2 coord) {\n"
      "  sample2D(tex, coord);\n"
      "}\n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, FunctionDefiniation) {
  const std::string str =
      "void clamp0() {\n"
      "  int a;\n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, FuncCall) {
  const std::string str =
      "void clamp1(int a) {}\n"
      "void clamp2(int a, int b) {}\n"
      "void clamp0() {\n"
      "  int a;\n"
      "  vec4 b = vec4(1, 2, 3, 4);\n"
      "  vec4 bn = normalize(b);\n"
      "  clamp1(1);\n"
      "  clamp1(a);\n"
      "  clamp2(1, a);\n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  dump_error(&context, std::cout);
}

TEST(AfxParser, SwitchCase) {
  const std::string str =
      "int clamp0(int a) {\n"
      " switch (a) {\n"
      "  case 1: return 0;\n"
      "  default: return 1;\n"
      "  }\n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, While) {
  const std::string str =
      "int clamp0(int a) {\n"
      " while(true) {     \n"
      "   a++;            \n"
      " };                \n"
      " while(true) a++;  \n"
      " return a;         \n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}


TEST(AfxParser, DoWhile) {
  const std::string str =
      "int clamp0(int a) {     \n"
      " do {                   \n"
      "   a++;                 \n"
      " } while (true);        \n"
      " do a++;  while(true);  \n"
      " return a;              \n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, ForStatement) {
  const std::string str =
      "int clamp0(int a) {                    \n"
      " int i = 0;                            \n"
      " for (; i < 10; i++) {a++;}            \n"
      " for (; i < 10; ) {a++;}               \n"
      " for (int j = 0; j < 10; ++j) a++;     \n"
      " for (int k = 0; k < 10; ++k) { a++;}  \n"
      " for (int k = 0; k < 10; ++k) { a++;}  \n"  // TODO(yanglei)
      " return a;                             \n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, Technique) {
  const std::string str =
      "technique [\n"
      "  name=\"effect\"\n"
      "  ps_main=\"ps_main\"\n"
      "  vs_main=\"vs_main\"\n"
      "];\n"
      ;
  ParseContext::Options opt;
  // nopt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, ArrayInit) {
  const std::string str = 
      "void func() {\n"
      "  int a[10]; \n"
      "  const int size = 10;\n"
      "  int b1[size];\n"
      "  int b2[size * 10 + 1 - 5 * 8 / 2];\n"
      "}\n";
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
}

TEST(AfxParser, BuiltIn) {
  const std::string str =
      "float dot(vec3 v1, vec3 v2) { return 1.0;}\n"
      "float dot(vec4 v1, vec4 v2) { return 1.0;}\n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  dump_error(&context, std::cout);
}

TEST(AfxParser, Swizzle) {
  const std::string str =
      "void test() {\n"
      "  vec4 v = vec4(1, 2, 3, 4);\n"
      "  vec4 v2 = v.xyz + v.yzw + v.rgb;\n"
      "  v2 = v.aaaa;\n"
      "  v2 = v.rrrr;\n"
      "  v2 = v.rgba;\n"
      "  v2 = v.wwww;\n"
      "  v2 = v.xyzw;\n"
      "}\n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  dump_error(&context, std::cout);
}

TEST(AfxParser, Assign) {
  const std::string str =
      "struct Input {\n"
      "  ivec4 index;\n"
      "  vec4 weights;\n"
      "};\n"
      "mat4 bone[100];\n"
      "void test() {\n"
      "  Input input;\n"
      "  float weight1 = input.weights[input.index[0]];\n"
      "  mat4 mat_0 = bone[input.index[1]] * weight1;\n"
      "  mat4 mat_1 = bone[input.index[1]] * input.weights[input.index[0]];\n"
      "  mat4 mat_2 = bone[input.index[1]] * input.weights[input.index[0]]"
      "    + mat_0;\n"
      "}\n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  dump_error(&context, std::cout);
}

TEST(AfxParser, Material) {
  const std::string str = ""
      "struct VSoutput {                              \n"
      "  vec3 position;                               \n"
      "  vec3 normal;                                 \n"
      "  vec2 texcoord[10];                           \n"
      "  Texture2D diffuse_tex[10];                   \n"
      "  Texture2D bump_tex[10];                      \n"
      "  Texture2D light_tex[10];                     \n"
      "};                                             \n"
      "vec4 CalaMaterial(VSoutput o) {                \n"
      "  return o.texcoord[0];                        \n"
      "}                                              \n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  DUMP_AFXCOMPILE_ERROR(context);
  // DumpASTree(&context, std::cout);
}

TEST(AfxParser, Sampler) {
  const std::string str =
      "struct VSoutput1 {                \n"
      "  vec3 position;                  \n"
      "  vec2 coord[3];                  \n"
      "  Texture2D tex[3];               \n"
      "};                                \n"
      "uniform VSoutput1 o;                    \n"
      "vec4 main() {                     \n"
      "  vec2 tex = o.tex[0];\n"
      "  return sample2D(o.tex[0], tex);\n"
      "}                                 \n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  opt.syntax_valid = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  // DumpASTree(&context, std::cout);
  DUMP_AFXCOMPILE_ERROR(context);
}

TEST(AfxParser, Simple1) {
  const std::string str = ""
      "technique [                                    \n"
      "  name=\"simple\"                              \n"
      "  ps_main=\"ps_main\"                          \n"
      "  vs_main=\"vs_main\"                          \n"
      "];                                             \n"
      "struct VSoutput {                              \n"
      "  [semantic=\"position\"]                      \n"
      "  vec3 position;                               \n"
      "  vec3 normal;                                 \n"
      "  vec2 texcoord;                               \n"
      "};                                             \n"
      "struct VSInput {                               \n"
      "  vec3 position;                               \n"
      "  vec3 normal;                                 \n"
      "  vec2 texcoord;                               \n"
      "};                                             \n"
      "VSoutput vsmain(VSInput input) {               \n"
      " VSoutput o;                                   \n"
      " o.position = input.position ;                 \n"
      " o.normal = input.normal;                      \n"
      " o.texcoord = o.texcoord;                      \n"
      " return o;                                     \n"
      "}                                              \n"
      "vec4 psmain(VSoutput o) {                      \n"
      "  return vec4(1, 1, 1, 1);                     \n"
      "}                                              \n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context(AFXL(""), "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  dump_error(&context, std::cout);
}
