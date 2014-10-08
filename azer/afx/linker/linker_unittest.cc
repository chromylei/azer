#include <map>
#include <string>

#include "azer/afx/afx.h"
#include "azer/afx/linker/linker.h"
#include "azer/afx/linker/testutil.h"
#include "azer/afx/compiler/testutil.h"
#include "azer/afx/util/file_loader.h"
#include "azer/base/string.h"
#include "base/files/file_path.h"
#include "base/strings/utf_string_conversions.h"
#include "gtest/gtest.h"

using azer::afx::AfxLinker;
using azer::afx::FileLoader;
using ::base::FilePath;

const std::string light_source = ""
    "#package Light;                \n"
    "struct SpotLight {             \n"
    " vec4 position;                \n"
    " vec3 normal;                  \n"
    "};                             \n"
    ;

const std::string effect_source = ""
    "#include \"azer/afx/light.afxi\"               \n"
    "technique [                                    \n"
    "  name=\"effect\"                              \n"
    "  ps_main=\"ps_main\"                          \n"
    "  vs_main=\"vs_main\"                          \n"
    "];                                             \n"
    "[uniform_func=\"SetLight\"]                    \n"
    "uniform Light::SpotLight light1;               \n"
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
    " o.position = input.position;                  \n"
    " o.normal = input.normal;                      \n"
    " o.texcoord = o.texcoord;                      \n"
    " return o;                                     \n"
    "}                                              \n"
    "vec4 psmain(VSoutput o) {                      \n"
    "  return vec4(1, 1, 1, 1);                     \n"
    "}                                              \n"
    ;

TEST(AfxLinkerTest, base) {
  std::vector<FilePath::StringType> inc;
  AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::MemoryFileLoader loader(inc);
  AfxLinker linker(&loader, opt);
  loader.AddFile(AZER_LITERAL("effect.afx"), effect_source);
  loader.AddFile(AZER_LITERAL("azer//afx//light.afxi"), light_source);
  EXPECT_TRUE(linker.Load(effect_source, AFXL("effect.afx")));
  DumpLinkError(&linker);
}

TEST(AfxLinkerTest, LoadFailed) {
  std::vector<FilePath::StringType> inc;
  AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::MemoryFileLoader loader(inc);
  AfxLinker linker(&loader, opt);
  loader.AddFile(AZER_LITERAL("effect.afx"), effect_source);
  ASSERT_FALSE(linker.Load(effect_source, AFXL("effect.afx")));
}

TEST(AfxLinkerTest, DependencyTree) {
}

TEST(AfxLinkerTest, MultiLevelInclude) {
  const FilePath::StringType kTestdataDir = AFXL("azer/afx/testdata/multi_level_include");
  std::vector<FilePath::StringType> inc;
  inc.push_back(kTestdataDir);
  AfxLinker::Options opt;
  opt.parse_astree = false;
  azer::afx::AfxParser parser(inc, opt);
  EXPECT_TRUE(parser.Parse(AFXL("azer/afx/testdata/multi_level_include/diffuse.afx")));
}
