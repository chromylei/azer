#include "azer/afx/compiler/astdump.h"
#include "azer/afx/compiler/testutil.h"
#include "azer/afx/linker/technique_parser.h"
#include "azer/afx/linker/linker.h"
#include "azer/afx/linker/testutil.h"
#include "base/file_util.h"
#include "gtest/gtest.h"

using azer::afx::TechniqueParser;
using azer::afx::ParseContext;
using azer::afx::AfxLinker;
using azer::afx::ASTreeDumper;
using azer::afx::FileLoader;
using ::base::FilePath;

const FilePath::StringType kTestdataDir = AFXL("azer/afx/testdata");

TEST(TechniqueParseTest, VertexShader) {
  std::vector<FilePath::StringType> inc;
  inc.push_back(kTestdataDir);
  AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::FileLoader loader(inc);
  AfxLinker linker(&loader, opt);
  std::string content;
  ASSERT_TRUE(::base::ReadFileToString(
      base::FilePath(AZER_LITERAL("azer/afx/testdata/diffuse.afx")), &content));
  EXPECT_TRUE(linker.Load(content, AFXL("diffuse.afx")));
  DumpLinkError(&linker);
  // Dump(&linker);

  ParseContext* context = linker.root()->GetContext();
  TechniqueParser tparser;
  ASSERT_TRUE(tparser.Parse(context));
  TechniqueParser::Technique* technique = NULL;
  ASSERT_TRUE(tparser.GetTechnique("effect", &technique));

  // two independent, VSInput and VSoutput
  ASSERT_EQ(technique->name, "effect");
  const TechniqueParser::StageInfo& shader = technique->shader[azer::kVertexStage];
  ASSERT_TRUE(shader.entry != NULL);
  ASSERT_EQ(shader.depend.size(), 2u);
  ASSERT_EQ(shader.depend[0]->type(), azer::afx::ASTNode::kStructDeclNode);
  ASSERT_EQ(shader.depend[1]->type(), azer::afx::ASTNode::kStructDeclNode);
  azer::afx::StructDeclNode* decl = shader.depend[0]->ToStructDeclNode();
  ASSERT_EQ(decl->struct_name(), "VSInput");
  decl = shader.depend[1]->ToStructDeclNode();
  ASSERT_EQ(decl->struct_name(), "VSOutput");
}

TEST(TechniqueParseTest, PiexelShader) {
  std::vector<FilePath::StringType> inc;
  inc.push_back(kTestdataDir);
  AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::FileLoader loader(inc);
  
  AfxLinker linker(&loader, opt);
  std::string content;
  ASSERT_TRUE(::base::ReadFileToString(
      base::FilePath(AZER_LITERAL("azer/afx/testdata/effect.afx")), &content));
  EXPECT_TRUE(linker.Load(content, AFXL("effect.afx")));
  DumpLinkError(&linker);
  // Dump(&linker);

  ParseContext* context = linker.root()->GetContext();
  TechniqueParser tparser;
  ASSERT_TRUE(tparser.Parse(context));
  TechniqueParser::Technique* technique = NULL;
  ASSERT_TRUE(tparser.GetTechnique("effect", &technique));

  ASSERT_EQ(technique->name, "effect");
  const TechniqueParser::StageInfo& shader = technique->shader[azer::kPixelStage];
  ASSERT_TRUE(shader.entry != NULL);
  ASSERT_EQ(shader.depend.size(), 1u);
  ASSERT_EQ(shader.uni_depend.size(), 1u);
  ASSERT_TRUE(shader.depend[0]->IsStructDeclNode());
  ASSERT_EQ(shader.depend[0]->ToStructDeclNode()->struct_name(), "VSOutput");
  ASSERT_TRUE(shader.uni_depend[0]->IsStructDeclNode());
  ASSERT_EQ(shader.uni_depend[0]->ToStructDeclNode()->struct_name(), "SpotLight");
  ASSERT_EQ(shader.uniforms.size(), 1u);
  ASSERT_TRUE(shader.uniforms[0]->IsDeclarationNode());
  ASSERT_TRUE(shader.uniforms[0]->last_child() != NULL);
  ASSERT_TRUE(shader.uniforms[0]->last_child()->IsSymbolNode());
  azer::afx::SymbolNode* symbol = shader.uniforms[0]->last_child()->ToSymbolNode();
  ASSERT_EQ(symbol->symbolname(), "light1");
}
