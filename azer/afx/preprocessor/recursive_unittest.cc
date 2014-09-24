// unittest purpose
// 1 include and defines
#include "azer/afx/preprocessor/parser.h"
#include "azer/afx/util/file_loader.h"
#include "preprocessor_tab.h"
#include "gtest/gtest.h"

using azer::afx::PreprocessorParser;
using azer::afx::FileLoader;
using ::base::FilePath;

class ParseDelegate : public PreprocessorParser::Delegate {
 public:
  ParseDelegate(FileLoader* loader) : loader_(loader) {
  }
  
  virtual void OnAddInclude(PreprocessorParser* parser, const FilePath& path) {
    PreprocessorParser* newparser = new PreprocessorParser(parser, this, false);
    std::string code;
    ::base::FilePath finallypath;
    ASSERT_TRUE(loader_->Load(path, &code, &finallypath));
    ASSERT_TRUE(newparser->Parse(code));
  }
 private:
  FileLoader* loader_;
  DISALLOW_COPY_AND_ASSIGN(ParseDelegate);
};

TEST(RecursiveTest, Base) {
  const char* effect_source = ""
      "#define AFX_EFFECT 0x0002\n"
      "#include \"azer/afx/light.afxi\"\n"
      ;
  const char* light_source = ""
      "#define AFX_LIGHT 0x0001\n"
      ;

  std::vector<FilePath::StringType> inc;
  azer::afx::MemoryFileLoader loader(inc);
  loader.AddFile(AZER_LITERAL("effect.afx"), effect_source);
  loader.AddFile(AZER_LITERAL("azer//afx//light.afxi"), light_source);

  ParseDelegate delegate(&loader);
  PreprocessorParser parser(&delegate, false);
  ASSERT_TRUE(parser.Parse(effect_source));
  ASSERT_EQ(parser.GetDefinedValue("AFX_EFFECT"), "0x0002");
  ASSERT_EQ(parser.GetDefinedValue("AFX_LIGHT"), "0x0001");
}

TEST(RecursiveTest, Overlap) {
  const char* effect_source = ""
      "#define AFX_EFFECT 0x0002\n"
      "#include \"azer/afx/light.afxi\"\n"
      "#define AFX_LIGHT 0x0003\n"
      ;
  const char* light_source = ""
      "#define AFX_LIGHT 0x0001\n"
      ;

  std::vector<FilePath::StringType> inc;
  azer::afx::MemoryFileLoader loader(inc);
  loader.AddFile(AZER_LITERAL("effect.afx"), effect_source);
  loader.AddFile(AZER_LITERAL("azer//afx//light.afxi"), light_source);

  ParseDelegate delegate(&loader);
  PreprocessorParser parser(&delegate, false);
  ASSERT_TRUE(parser.Parse(effect_source));
  ASSERT_EQ(parser.GetDefinedValue("AFX_EFFECT"), "0x0002");
  ASSERT_EQ(parser.GetDefinedValue("AFX_LIGHT"), "0x0003");
}
