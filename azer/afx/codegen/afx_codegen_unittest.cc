#include "azer/afx/afx.h"
#include "base/file_util.h"
#include "gtest/gtest.h"

using azer::afx::AfxLinker;
using azer::afx::AfxCodegen;
using azer::afx::ASTreeDumper;
using azer::afx::FileLoader;
using azer::afx::ParseContext;
using azer::afx::TechniqueParser;

const std::string kTestdataDir = "azer/afx/testdata";

TEST(AfxCodegen, EffectVertex) {
  const std::string expect = ""
      "// using row_major\n"
      "#pragma pack_matrix(row_major)\n\n"
      "struct VSOutput {"
      "float3 position:SV_POSITION;"
      "float3 normal:NORMAL;"
      "float2 texcoord:TEXCOORD;};"
      "struct VSInput {"
      "float3 position:POSITION;"
      "float3 normal:NORMAL;"
      "float3 texcoord:TEXCOORD;};"
      "VSOutput vsmain(VSInput input) {"
      "VSOutput o;"
      "o.position = input.position;"
      "o.normal = input.normal;"
      "o.texcoord = input.texcoord;"
      "return o;}";
  std::vector<std::string> inc;
  inc.push_back(kTestdataDir);
  AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::AfxParser parser(inc, opt);
  ASSERT_TRUE(parser.Parse("azer/afx/testdata/diffuse.afx"));

  LOG_IF(ERROR, !parser.success()) << parser.GetErrorText();
  ParseContext* context = parser.GetContext();
  DUMP_AFXCOMPILE_ERROR(*context);
  TechniqueParser::Technique* technique = parser.GetTechnique("effect");
  ASSERT_TRUE(NULL != technique);
  ASSERT_EQ(technique->name, "effect");
  const TechniqueParser::StageInfo& shader = technique->shader[azer::kVertexStage];
  ASSERT_TRUE(shader.entry != NULL);

  azer::afx::HLSLCodeGeneratorFactory gen_factory;
  AfxCodegen codegen(&gen_factory);
  ASSERT_EQ(codegen.GenCode(azer::kVertexStage, shader), expect);
  // LOG(ERROR) << codegen.GenCode(shader);
}

TEST(AfxCodegen, EffectPixel) {
  const std::string expect = ""
      "// using row_major\n#pragma pack_matrix(row_major)\n\n"
      "struct VSOutput {"
      "float3 position:SV_POSITION;"
      "float3 normal:NORMAL;"
      "float2 texcoord:TEXCOORD;};"
      "struct afx__SpotLight {float4 position;float4 direction;float4 diffuse;};"
      "cbuffer c_buffer { afx__SpotLight light1;};"
      "float4 psmain(VSOutput o):SV_TARGET {"
      "return normalize(light1.diffuse) * dot(float4(o.normal, 1), light1.direction);}";
  std::vector<std::string> inc;
  inc.push_back(kTestdataDir);
  AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::AfxParser parser(inc, opt);
  ASSERT_TRUE(parser.Parse("azer/afx/testdata/effect.afx"));

  LOG_IF(ERROR, !parser.success()) << parser.GetErrorText();
  ParseContext* context = parser.GetContext();
  DUMP_AFXCOMPILE_ERROR(*context);

  TechniqueParser::Technique* technique = parser.GetTechnique("effect");
  ASSERT_TRUE(NULL != technique);
  ASSERT_EQ(technique->name, "effect");
  const TechniqueParser::StageInfo& shader = technique->shader[azer::kPixelStage];
  ASSERT_TRUE(shader.entry != NULL);

  azer::afx::HLSLCodeGeneratorFactory gen_factory;
  AfxCodegen codegen(&gen_factory);
  ASSERT_EQ(codegen.GenCode(azer::kPixelStage, shader), expect);
}

TEST(AfxCodegen, EffectPixelWithSampler) {
  const std::string expect = ""
      "// using row_major\n"
      "#pragma pack_matrix(row_major)\n\n"
      "struct VSOutput {"
      "float3 position:SV_POSITION;"
      "float3 normal:NORMAL;"
      "float2 texcoord:TEXCOORD;"
      "};"
      "struct afx__SpotLight {"
      "float4 position;"
      "float4 direction;"
      "float4 diffuse;"
      "};"
      "cbuffer c_buffer { "
      "afx__SpotLight light1;"
      "};"
      "Texture2D difftex;\n"
      "SamplerState difftex__d3d_sampler;\n"
      "float4 psmain(VSOutput o):SV_TARGET {"
      "return (normalize(light1.diffuse) * "
      "dot(float4(o.normal, 1), light1.direction)) * "
      "difftex.Sample(difftex__d3d_sampler, o.texcoord);}"
      ;

  std::vector<std::string> inc;
  inc.push_back(kTestdataDir);
  AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::AfxParser parser(inc, opt);
  ASSERT_TRUE(parser.Parse("azer/afx/testdata/texeffect.afx"));

  LOG_IF(ERROR, !parser.success()) << parser.GetErrorText();
  ParseContext* context = parser.GetContext();
  DUMP_AFXCOMPILE_ERROR(*context);
  

  TechniqueParser::Technique* technique = parser.GetTechnique("effect");
  ASSERT_TRUE(NULL != technique);
  ASSERT_EQ(technique->name, "effect");
  const TechniqueParser::StageInfo& shader = technique->shader[azer::kPixelStage];
  ASSERT_TRUE(shader.entry != NULL);

  azer::afx::HLSLCodeGeneratorFactory gen_factory;
  AfxCodegen codegen(&gen_factory);
  ASSERT_EQ(codegen.GenCode(azer::kPixelStage, shader), expect);
}

TEST(AfxCodegen, TextureAsParameter) {
}

TEST(AfxCodegen, TextureInStructure) {
  const std::string expect = ""
      "// using row_major\n"
      "#pragma pack_matrix(row_major)\n\n"
      "struct VSOutput {"
      "float3 position:SV_POSITION;"
      "float3 normal:NORMAL;"
      "float2 texcoord:TEXCOORD;"
      "};"
      "struct afx__SpotLight {"
      "float4 position;"
      "float4 direction;"
      "float4 diffuse;"
      "};"
      "cbuffer c_buffer { "
      "afx__SpotLight light1;"
      "};"
      "Texture2D mtrl__difftex[3];\n"
      "SamplerState mtrl__difftex__d3d_sampler[3];\n"
      "float4 psmain(VSOutput o):SV_TARGET {"
      "return (normalize(light1.diffuse) * "
      "dot(float4(o.normal, 1), light1.direction)) * "
      "mtrl__difftex[0].Sample(mtrl__difftex__d3d_sampler[0], o.texcoord);}"
      ;

  std::vector<std::string> inc;
  inc.push_back(kTestdataDir);
  AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::AfxParser parser(inc, opt);
  ASSERT_TRUE(parser.Parse("azer/afx/testdata/mtrl.afx"));

  LOG_IF(ERROR, !parser.success()) << parser.GetErrorText();
  ParseContext* context = parser.GetContext();
  DUMP_AFXCOMPILE_ERROR(*context);

  TechniqueParser::Technique* technique = NULL;
  technique = parser.GetTechnique("effect");
  ASSERT_TRUE(technique != NULL);
  ASSERT_EQ(technique->name, "effect");
  const TechniqueParser::StageInfo& shader = technique->shader[azer::kPixelStage];
  ASSERT_TRUE(shader.entry != NULL);

  azer::afx::HLSLCodeGeneratorFactory gen_factory;
  AfxCodegen codegen(&gen_factory);
  ASSERT_EQ(codegen.GenCode(azer::kPixelStage, shader), expect);
}

TEST(AfxCodegen, LineEffect) {
  const std::string expect = ""
      "// using row_major\n"
      "#pragma pack_matrix(row_major)\n\n"
      "struct VsOutput {"
      "float4 position:SV_POSITION;"
      "};"
      "cbuffer c_buffer { "
      "float4 diffuse;"
      "};"
      "float4 ps_main(VsOutput o):SV_TARGET {"
      "return diffuse;"
      "}"
      ;

  std::vector<std::string> inc;
  inc.push_back(kTestdataDir);
  AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::AfxParser parser(inc, opt);
  EXPECT_TRUE(parser.Parse("azer/afx/testdata/line.afx"));

  LOG_IF(ERROR, !parser.success()) << parser.GetErrorText();
  ParseContext* context = parser.GetContext();
  DUMP_AFXCOMPILE_ERROR(*context);

  TechniqueParser::Technique* technique = NULL;
  technique = parser.GetTechnique("line");
  ASSERT_TRUE(technique != NULL);
  ASSERT_EQ(technique->name, "line");
  const TechniqueParser::StageInfo& shader = technique->shader[azer::kPixelStage];
  ASSERT_TRUE(shader.entry != NULL);

  azer::afx::HLSLCodeGeneratorFactory gen_factory;
  AfxCodegen codegen(&gen_factory);
  ASSERT_EQ(codegen.GenCode(azer::kPixelStage, shader), expect);
}

TEST(AfxCodegen, Shadowmap) {
  const std::string expect = ""
      "// using row_major\n"
      "#pragma pack_matrix(row_major)\n\n"
      "bool afx__InShadow(Texture2D smtex, SamplerState smtex__d3d_sampler, "
      "float4 sm_position) {return true;}"
      "struct VsOutput {"
      "float4 position:SV_POSITION;"
      "float4 sm_position:SM_POSITION;};"
      "Texture2D shadowmap_tex;\n"
      "SamplerState shadowmap_tex__d3d_sampler;\n"
      "float4 ps_main(VsOutput o):SV_TARGET {"
      "if (afx__InShadow(shadowmap_tex, shadowmap_tex__d3d_sampler, "
      "o.sm_position)){return float4(1, 1, 1, 1);} "
      "else {return float4(1, 1, 1, 1);}}"
      ;

  std::vector<std::string> inc;
  inc.push_back(kTestdataDir);
  AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::AfxParser parser(inc, opt);
  EXPECT_TRUE(parser.Parse("azer/afx/testdata/shadowmap.afx"));

  LOG_IF(ERROR, !parser.success()) << parser.GetErrorText();
  ParseContext* context = parser.GetContext();
  DUMP_AFXCOMPILE_ERROR(*context);

  TechniqueParser::Technique* technique = NULL;
  technique = parser.GetTechnique("diffuse");
  ASSERT_TRUE(technique != NULL);
  ASSERT_EQ(technique->name, "diffuse");
  const TechniqueParser::StageInfo& shader = technique->shader[azer::kPixelStage];
  ASSERT_TRUE(shader.entry != NULL);

  azer::afx::HLSLCodeGeneratorFactory gen_factory;
  AfxCodegen codegen(&gen_factory);
  ASSERT_EQ(codegen.GenCode(azer::kPixelStage, shader), expect);
}

