#include "gtest/gtest.h"
#include "azer/afx/compiler/astdump.h"
#include "azer/afx/compiler/parser.h"
#include "azer/afx/compiler/testutil.h"
#include "azer/afx/codegen/hlsl_codegen.h"
#include "azer/afx/codegen/code_generator.h"

using azer::afx::ASTNode;
using azer::afx::ASTNodeFactory;
using azer::afx::ParseContext;
using azer::afx::Parser;
using azer::afx::CodeGenerator;
using azer::afx::SnippetCodeGenerator;
using azer::afx::HLSLCodeGeneratorFactory;

TEST(HLSLCodeGenerator, Expression) {
  const std::string expect =
      "void psmain() {"
      "int a = 0;100;"
      "++a;--a;-a;++(++(++a));"
      "++(++(++a));-(++a);bool b = false;"
      "bool c = (true && b) || (!b);"
      "float f = (-1.00000f) / 100.000f;"
      "}"
      ;
  const std::string str =
      "void psmain() {\n"
      "  int a = 0;\n"
      "  100;a++;--a;-a;a++++++;++++++a;-+++a;\n"
      "  bool b = false;\n"
      "  bool c = true && b || !b;\n"
      "  float f = -1.0f / 100.0f;\n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect);
  // LOG(ERROR) << generator.GetCode();
}

TEST(HLSLCodeGenerator, VariableDecl) {
  const std::string expect =
      "void psmain() {"
      "bool b = false;"
      "char c = 'a';"
      "int ia, ib[3], ic = 4;"
      "float farr1[3];"
      "float farr2[3] = {1, 2, 3};"
      "}"
      ;
  const std::string str =
      "void psmain() {                \n"
      "  bool b = false;              \n"
      "  char c = 'a';                \n"
      "  int ia, ib[3], ic=4;         \n"
      "  float farr1[3];              \n"
      "  float farr2[3] = {1, 2, 3};  \n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect);
  // LOG(ERROR) << generator.GetCode();
}

TEST(HLSLCodeGenerator, ArrayMember) {
  const std::string expect =
      "void psmain() {"
      "float farr[3] = {1, 2, 3};"
      "float farr_1 = farr[0];"
      "}"
      ;
  const std::string str =
      "void psmain() {                \n"
      "  float farr[3] = {1, 2, 3};   \n"
      "  float farr_1 = farr[0];\n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect);
  // LOG(ERROR) << generator.GetCode();
}

TEST(HLSLCodeGenerator, StructureDecl) {
  const std::string expect_str =
      "struct VSoutput1 {float3 position;float3 normal;float2 texcoord;};";
  const std::string str =
      "struct VSoutput1 {\n"
      "  vec3 position;\n"
      "  vec3 normal;\n"
      "  vec2 texcoord;\n"
      "};\n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect_str);
}

TEST(HLSLCodeGenerator, FuncCall) {
  const std::string expect =
      "void clamp1(int a) {}"
      "void clamp2(int a, int b) {}"
      "void clamp0() {int a;clamp1(a);clamp1(1);clamp2(1, a);}";
  const std::string str =
      "void clamp1(int a) {}\n"
      "void clamp2(int a, int b) {}\n"
      "void clamp0() {\n"
      "  int a;\n"
      "  clamp1(a);\n"
      "  clamp1(1);\n"
      "  clamp2(1, a);\n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect);
}

TEST(HLSLCodeGenerator, IfElse) {
  const std::string expect =
      "float4 main() {"
      "int a = 0;if ((a == 0) && true){a = 1;} else {a = 2;}"
      "if ((a == 0) || false){a = 1;} else {a = 2;}"
      "if (a == 0){a = 1;} else {a = 2;}}";
  const std::string str =
      " vec4 main() {\n"
      "  int a = 0;  \n"
      "  if (a == 0 && true) a = 1; else a = 2;\n"
      "  if (a == 0 || false) a = 1; else { a = 2;}\n"
      "  if (a == 0) {a = 1; } else { a = 2;}\n"
      "}\n"
      ;

  azer::afx::ParseContext::Options opt;
  opt.dump_parser = false;
  opt.dump_tokenizer = false;
  azer::afx::ASTNodeFactory factory;
  azer::afx::ParseContext context("", "", str, &factory, opt);
  azer::afx::Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect);
  // LOG(ERROR) << generator.GetCode();
}

TEST(HLSLCodeGenerator, While) {
  const std::string expect =
      "int clamp0(int a) {while(true){++a;}while(true){while(true){++a;}}return a;}";
  const std::string str =
      "int clamp0(int a) {\n"
      " while(true) a++;  \n"
      " while(true) {     \n"
      " while(true) a++;  \n"
      "   a++;            \n"
      " };                \n"
      " return a;         \n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect);
}


TEST(HLSLCodeGenerator, DoWhile) {
  const std::string expect =
      "int clamp0(int a) {do{++a;}while(true);do{++a;}while(true);return a;}";
  const std::string str =
      "int clamp0(int a) {     \n"
      " do a++;  while(true);  \n"
      " do {                   \n"
      "   a++;                 \n"
      " } while (true);        \n"
      " return a;              \n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect);
}

TEST(HLSLCodeGenerator, ForStatement) {
  const std::string expect =
      "int clamp0(int a) {"
      "int i = 0;"
      "for(;i < 10;++i){++a;}"
      "for(;i < 10;){++a;}"
      "for(int j = 0;j < 10;++j){++a;}"
      "for(int k = 100;k > 10;--k){++a;}"
      "for(int k = 0;k < 10;++k){++a;}"
      "return a;}"
      ;
  const std::string str =
      "int clamp0(int a) {                     \n"
      " int i = 0;                             \n"
      " for (; i < 10; i++) {a++;}             \n"
      " for (; i < 10; ) {a++;}                \n"
      " for (int j = 0; j < 10; ++j) a++;      \n"
      " for (int k = 100; k > 10; --k) { a++;} \n"
      " for (int k = 0; k < 10; ++k) { a++;}   \n"  // TODO(yanglei)
      " return a;                              \n"
      "}"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory, opt);
  Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect);
}

TEST(HLSLCodeGenerator, Texture) {
  const std::string expect =
      "Texture2D texdiffuse;\n"
      "SamplerState texdiffuse__d3d_sampler;\n"
      "void psmain() {texdiffuse.Sample(texdiffuse__d3d_sampler, float2(1, 1));}"
      ;
  const std::string str =
      "uniform Texture2D texdiffuse;\n"
      "void psmain() {\n"
      "  sample2D(texdiffuse, vec2(1, 1));\n"
      "}\n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  DUMP_AFXCOMPILE_ERROR(context);

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect);
}

TEST(HLSLCodeGenerator, TextureAsParam) {
  const std::string expect =
      "Texture2D tex1;\n"
      "SamplerState tex1__d3d_sampler;\n"
      "float4 sample(Texture2D tex, SamplerState tex__d3d_sampler, float2 coord) {"
      "return tex.Sample(tex__d3d_sampler, coord);}"
      "float4 psmain() {"
      "return sample(tex1, tex1__d3d_sampler, float2(1.00000f, 1.00000f));}"
      ;
  const std::string str =
      "uniform Texture2D tex1;\n"
      "vec4 sample(Texture2D tex, vec2 coord) {return sample2D(tex, coord);}\n"
      "vec4 psmain() {\n"
      "  return sample(tex1, vec2(1.0f, 1.0f));\n"
      "}\n"
      ;
  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  DUMP_AFXCOMPILE_ERROR(context);

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect);
}

TEST(HLSLCodeGenerator, Material) {
  const std::string expect = ""
      "struct Material {"
      "float3 position;"
      "float2 coord[3];"
      "Texture2D tex[3];"
      "}; "
      "Material mtrl;"
      "float4 main() {"
      "return mtrl__tex[0].Sample(mtrl__tex__d3d_sampler[0], mtrl.coord[0]);"
      "}";
  const std::string str =
      "struct Material {                 \n"
      "  vec3 position;                  \n"
      "  vec2 coord[3];                  \n"
      "  Texture2D tex[3];               \n"
      "};                                \n"
      "uniform Material mtrl;                    \n"
      "vec4 main() {                     \n"
      "  return sample2D(mtrl.tex[0], mtrl.coord[0]);\n"
      "}                                 \n"
      ;

  ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  ASTNodeFactory factory;
  ParseContext context("", "", str, &factory, opt);
  Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  DUMP_AFXCOMPILE_ERROR(context);
  // DumpASTree(&context, std::cout);

  HLSLCodeGeneratorFactory gen_factory;
  SnippetCodeGenerator generator(&gen_factory);
  generator.GenCode(context.root());
  ASSERT_EQ(generator.GetCode(), expect);
}
