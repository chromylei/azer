#include "azer/afx/preprocessor/tokenizer.h"
#include "azer/afx/preprocessor/parser.h"
#include "base/logging.h"
#include "gtest/gtest.h"
#include "preprocessor_tab.h"

using azer::afx::CodeParty;
using azer::afx::PreprocessorTokenizer;

TEST(CodeParty, Comments) {
  const std::string code = ""
      "// hello, this is a test for tokenizer\n"
      "/**\n"
      " * this is a test for tokenizer\n"
      " **/\n";
  std::string content;
  CodeParty tokenizer(code);
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCommentPart);
  ASSERT_EQ(content, "// hello, this is a test for tokenizer\n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCommentPart);
  ASSERT_EQ(content, "/**\n * this is a test for tokenizer\n **/");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCodePart);
  ASSERT_EQ(content, "\n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kEndPart);
}

TEST(CodeParty, Preprocessor) {
  const std::string code = ""
      "#define HELLO_WORLD2() \"hello\"\n"
      "#define HELLO_WORLD() \"hello\"";
  std::string content;
  CodeParty tokenizer(code);
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kPreprocessorPart);
  ASSERT_EQ(content, "#define HELLO_WORLD2() \"hello\"\n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kPreprocessorPart);
  ASSERT_EQ(content, "#define HELLO_WORLD() \"hello\"");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kEndPart);
}

TEST(CodeParty, PreprocessorMultiLine) {
  const std::string code = ""
      "#define HELLO_WORLD() \"hello\" \\\n"
      "hello\n";
  std::string content;
  CodeParty tokenizer(code);
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kPreprocessorPart);
  ASSERT_EQ(content, "#define HELLO_WORLD() \"hello\" \\\nhello\n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kEndPart);
}

TEST(CodeParty, CodeWithComments) {
  const std::string code = ""
      "/**\n"
      " * this is a test for tokenizer\n"
      " **/\n"
      " void main() {\n"
      "  /* return code */\n"
      "  return 0; // return\n"
      "}\n"
      ;
  std::string content;
  CodeParty tokenizer(code);
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCommentPart);
  ASSERT_EQ(content, "/**\n * this is a test for tokenizer\n **/");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCodePart);
  ASSERT_EQ(content, "\n void main() {\n  ");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCommentPart);
  ASSERT_EQ(content, "/* return code */");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCodePart);
  ASSERT_EQ(content, "\n  return 0; ");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCommentPart);
  ASSERT_EQ(content, "// return\n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCodePart);
  ASSERT_EQ(content, "}\n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kEndPart);
}

TEST(CodeParty, Sample) {
  const std::string code = ""
      "/**\n"
      " * this is a test for tokenizer\n"
      " **/\n"
      " void main() {\n"
      "# ifdef WIN32_\n"
      "  /* return code */\n"
      "  return 0; // return\n"
      "  #else \n"
      "  return 1;\n"
      "#  endif  \n"
      "}\n"
      ;
  std::string content;
  CodeParty tokenizer(code);
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCommentPart);
  ASSERT_EQ(content, "/**\n * this is a test for tokenizer\n **/");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCodePart);
  ASSERT_EQ(content, "\n void main() {\n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kPreprocessorPart);
  ASSERT_EQ(content, "# ifdef WIN32_\n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCodePart);
  ASSERT_EQ(content, "  ");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCommentPart);
  ASSERT_EQ(content, "/* return code */");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCodePart);
  ASSERT_EQ(content, "\n  return 0; ");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCommentPart);
  ASSERT_EQ(content, "// return\n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCodePart);
  ASSERT_EQ(content, "  ");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kPreprocessorPart);
  ASSERT_EQ(content, "#else \n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCodePart);
  ASSERT_EQ(content, "  return 1;\n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kPreprocessorPart);
  ASSERT_EQ(content, "#  endif  \n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kCodePart);
  ASSERT_EQ(content, "}\n");
  EXPECT_EQ(tokenizer.GetNextToken(&content), CodeParty::kEndPart);
}

TEST(PreprocessorTokenizer, Token) {
  const std::string code = ""
      "#define HELLO_WORLD2() \"hello\"\n";
  const char* token[] = {"#", "define", "HELLO_WORLD2", "(", ")", "hello"};
  int lex[] = {PRE_SHARP, PRE_DEFINE, PRE_STRING, PRE_LEFT_PAREN, PRE_RIGHT_PAREN,
             PRE_STRING};
  PreprocessorTokenizer tokenizer(code);
  for (int cnt = 0; cnt < arraysize(lex); ++cnt) {
    YYSTYPE yylval = {0};
    int tok = tokenizer.GetNextToken(&yylval);
    EXPECT_EQ(tok, lex[cnt]);
  }
}
