#include "gtest/gtest.h"
#include "azer/afx/compiler/parser.h"
#include "azer/afx/compiler/astdump.h"
#include "azer/afx/compiler/testutil.h"

using azer::afx::ASTNode;
using azer::afx::ASTNodeVec;

TEST(ASTree, Expression1) {
  const std::string str =
      "void psmain() {\n"
      "  int a = 0;\n"
      "  100;a++;--a;-a;a++++++;++++++a;++-+a;\n"
      "  }"
      ;
  azer::afx::ParseContext::Options opt;
  opt.dump_parser = false;
  opt.dump_tokenizer = false;
  azer::afx::ASTNodeFactory factory;
  azer::afx::ParseContext context("", "", str, &factory, opt);
  azer::afx::Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
  DumpASTree(&context, std::cout);
}

TEST(ASTree, StructureDecl) {
  const std::string expect =
      "ScopedNode\n"
      " StructDeclNode\n"
      "  FieldNode[position]\n"
      "  FieldNode[normal]\n"
      "  FieldNode[texcoord]\n"
      ;
  const std::string str =
      "/* struct defection\n"
      " *\n"
      "*/\n"
      "struct VSoutput1 {\n"
      "  vec3 position;\n"
      "  vec3 normal;\n"
      "  vec2 texcoord;\n"
      "};\n";

  azer::afx::ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  azer::afx::ASTNodeFactory factory;
  azer::afx::ParseContext context("", "", str, &factory, opt);
  azer::afx::Parser parser;
  ASSERT_TRUE(parser.Parse(&context));
  std::string dump;
  azer::afx::ASTreeDumper dumper;
  dumper.Dump(context.root(), &dump);
  // LOG(ERROR) << dump;
  ASSERT_EQ(dump, expect);
}

TEST(ASTree, FunctionDefinition) {
  const std::string expect =
      "ScopedNode\n"
      " FuncDefNode{main}\n"
      "  FuncProtoNode{main}\n"
      "  ScopedNode\n"
      "   StatementNode\n"
      "    ReturnNode\n"
      "     FuncCallTypeInitNode{vector4}\n"
      "      ConstNode{1}\n"
      "      ConstNode{2}\n"
      "      ConstNode{3}\n"
      "      ConstNode{4}\n"
      ;
  const std::string str =
      " vec4 main();\n"
      " vec4 main() {\n"
      "  return vec4(1, 2, 3, 4);\n"
      "}\n"
      ;
  azer::afx::ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  azer::afx::ASTNodeFactory factory;
  azer::afx::ParseContext context("", "", str, &factory, opt);
  azer::afx::Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  std::string dump;
  azer::afx::ASTreeDumper dumper;
  dumper.Dump(context.root(), &dump);
  // LOG(ERROR) << dump;
  ASSERT_EQ(dump, expect);
}

TEST(ASTree, IfElse) {
  const std::string expect =
      "ScopedNode\n"
      " FuncDefNode{main}\n"
      "  FuncProtoNode{main}\n"
      "  ScopedNode\n"
      "   StatementNode\n"
      "    DeclarationNode\n"
      "     SymbolNode{a}\n"
      "      TypedNode\n"
      "      InitializerNode\n"
      "       ConstNode{0}\n"
      "   StatementNode\n"
      "    IfElseNode\n"
      "     ScopedNode\n"
      "      StatementNode\n"
      "       BinaryOpNode[=]\n"
      "        RefSymbolNode[a]\n"
      "        ConstNode{1}\n"
      "     ScopedNode\n"
      "      StatementNode\n"
      "       BinaryOpNode[=]\n"
      "        RefSymbolNode[a]\n"
      "        ConstNode{2}\n"
      "     BinaryOpNode[==]\n"
      "      RefSymbolNode[a]\n"
      "      ConstNode{0}\n"
      "   StatementNode\n"
      "    IfElseNode\n"
      "     ScopedNode\n"
      "      StatementNode\n"
      "       BinaryOpNode[=]\n"
      "        RefSymbolNode[a]\n"
      "        ConstNode{1}\n"
      "     ScopedNode\n"
      "      StatementNode\n"
      "       BinaryOpNode[=]\n"
      "        RefSymbolNode[a]\n"
      "        ConstNode{2}\n"
      "     BinaryOpNode[==]\n"
      "      RefSymbolNode[a]\n"
      "      ConstNode{0}\n"
      "   StatementNode\n"
      "    IfElseNode\n"
      "     ScopedNode\n"
      "      StatementNode\n"
      "       BinaryOpNode[=]\n"
      "        RefSymbolNode[a]\n"
      "        ConstNode{1}\n"
      "     ScopedNode\n"
      "      StatementNode\n"
      "       BinaryOpNode[=]\n"
      "        RefSymbolNode[a]\n"
      "        ConstNode{2}\n"
      "     BinaryOpNode[==]\n"
      "      RefSymbolNode[a]\n"
      "      ConstNode{0}\n"
      ;

  const std::string str =
      " vec4 main() {\n"
      "  int a = 0;  \n"
      "  if (a == 0) a = 1; else a = 2;\n"
      "  if (a == 0) a = 1; else { a = 2;}\n"
      "  if (a == 0) {a = 1; } else { a = 2;}\n"
      "}\n"
      ;

  azer::afx::ParseContext::Options opt;
  // opt.dump_parser = false;
  // opt.dump_tokenizer = false;
  azer::afx::ASTNodeFactory factory;
  azer::afx::ParseContext context("", "", str, &factory, opt);
  azer::afx::Parser parser;
  EXPECT_TRUE(parser.Parse(&context));
  dump_error(&context, std::cout);

  std::string dump;
  azer::afx::ASTreeDumper dumper;
  dumper.Dump(context.root(), &dump);
  EXPECT_EQ(dump, expect);
  // LOG(ERROR) << dump;
}

TEST(ASTree, IfElse2) {
  const std::string expect =
      "ScopedNode\n"
      " FuncDefNode{main}\n"
      "  FuncProtoNode{main}\n"
      "  ScopedNode\n"
      "   StatementNode\n"
      "    DeclarationNode\n"
      "     SymbolNode{a}\n"
      "      TypedNode\n"
      "      InitializerNode\n"
      "       ConstNode{0}\n"
      "   StatementNode\n"
      "    IfElseNode\n"
      "     ScopedNode\n"
      "      StatementNode\n"
      "       ReturnNode\n"
      "        ConstNode{0}\n"
      "     ScopedNode\n"
      "      StatementNode\n"
      "       IfElseNode\n"
      "        ScopedNode\n"
      "         StatementNode\n"
      "          ReturnNode\n"
      "           ConstNode{1}\n"
      "        ScopedNode\n"
      "         StatementNode\n"
      "          IfElseNode\n"
      "           ScopedNode\n"
      "            StatementNode\n"
      "             ReturnNode\n"
      "              ConstNode{1}\n"
      "           ScopedNode\n"
      "            StatementNode\n"
      "             ReturnNode\n"
      "              ConstNode{3}\n"
      "           BinaryOpNode[==]\n"
      "            RefSymbolNode[a]\n"
      "            ConstNode{2}\n"
      "        BinaryOpNode[==]\n"
      "         RefSymbolNode[a]\n"
      "         ConstNode{1}\n"
      "     BinaryOpNode[==]\n"
      "      RefSymbolNode[a]\n"
      "      ConstNode{0}\n"
      ;
  const std::string str =
      " vec4 main() {      \n"
      "  int a = 0;        \n"
      "  if (a == 0)       \n"
      "    return 0;       \n"
      "  else if (a == 1)  \n"
      "    return 1;       \n"
      "  else if (a == 2) {\n"
      "    return 1;       \n"
      "  } else {          \n"
      "    return 3;       \n"
      "  }                 \n"
      "}\n"
      ;

  azer::afx::ParseContext::Options opt;
  opt.dump_parser = false;
  opt.dump_tokenizer = false;
  azer::afx::ASTNodeFactory factory;
  azer::afx::ParseContext context("", "", str, &factory, opt);
  azer::afx::Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  std::string dump;
  azer::afx::ASTreeDumper dumper;
  // LOG(ERROR) << dump;
  dumper.Dump(context.root(), &dump);
  ASSERT_EQ(dump, expect);
}

TEST(ASTree, ForLoop) {
  const std::string expect =
      "ScopedNode\n"
      " FuncDefNode{main}\n"
      "  FuncProtoNode{main}\n"
      "  ScopedNode\n"
      "   StatementNode\n"
      "    DeclarationNode\n"
      "     SymbolNode{a}\n"
      "      TypedNode\n"
      "      InitializerNode\n"
      "       ConstNode{0}\n"
      "   StatementNode\n"
      "    ForLoopNode\n"
      "     ScopedNode\n"
      "      DeclarationNode\n"
      "       SymbolNode{i}\n"
      "        TypedNode\n"
      "        InitializerNode\n"
      "         ConstNode{0}\n"
      "      BinaryOpNode[<]\n"
      "       RefSymbolNode[i]\n"
      "       ConstNode{10}\n"
      "      UnaryOpNode[++]\n"
      "       RefSymbolNode[i]\n"
      "      StatementNode\n"
      "       UnaryOpNode[++]\n"
      "        RefSymbolNode[a]\n"
      ;

  const std::string str =
      " vec4 main() {\n"
      "  int a = 0;  \n"
      "  for (int i = 0; i < 10; ++i) { a++;}\n"
      "}\n"
      ;

  azer::afx::ParseContext::Options opt;
  opt.dump_parser = false;
  opt.dump_tokenizer = false;
  azer::afx::ASTNodeFactory factory;
  azer::afx::ParseContext context("", "", str, &factory, opt);
  azer::afx::Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  std::string dump;
  azer::afx::ASTreeDumper dumper;
  dumper.Dump(context.root(), &dump);
  ASSERT_EQ(dump, expect);
  // LOG(ERROR) << dump;
}

TEST(ASTree, WhileLoop) {
  const std::string expect =
      "ScopedNode\n"
      " FuncDefNode{main}\n"
      "  FuncProtoNode{main}\n"
      "  ScopedNode\n"
      "   StatementNode\n"
      "    DeclarationNode\n"
      "     SymbolNode{a}\n"
      "      TypedNode\n"
      "      InitializerNode\n"
      "       ConstNode{0}\n"
      "   StatementNode\n"
      "    DeclarationNode\n"
      "     SymbolNode{i}\n"
      "      TypedNode\n"
      "      InitializerNode\n"
      "       ConstNode{0}\n"
      "   StatementNode\n"
      "    WhileLoopNode\n"
      "     StatementNode\n"
      "      UnaryOpNode[++]\n"
      "       RefSymbolNode[i]\n"
      "     BinaryOpNode[<]\n"
      "      RefSymbolNode[i]\n"
      "      ConstNode{10}\n"
      "   StatementNode\n"
      "    WhileLoopNode\n"
      "     StatementNode\n"
      "      UnaryOpNode[++]\n"
      "       RefSymbolNode[i]\n"
      "     StatementNode\n"
      "      UnaryOpNode[++]\n"
      "       RefSymbolNode[a]\n"
      "     BinaryOpNode[<]\n"
      "      RefSymbolNode[i]\n"
      "      ConstNode{10}\n"
      ;
  const std::string str =
      " void main() {       \n"
      "  int a = 0;         \n"
      "  int i = 0;         \n"
      "  while (i < 10) i++;\n"
      "  while (i < 10) {\n"
      "    i++;\n"
      "    a++;\n"
      "  }\n"
      "}\n"
      ;

  azer::afx::ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = false;
  azer::afx::ASTNodeFactory factory;
  azer::afx::ParseContext context("", "", str, &factory, opt);
  azer::afx::Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  std::string dump;
  azer::afx::ASTreeDumper dumper;
  dumper.Dump(context.root(), &dump);
  ASSERT_EQ(dump, expect);
  // LOG(ERROR) << dump;
}

TEST(ASTree, DoWhileLoop) {
  const std::string expect =
      "ScopedNode\n"
      " FuncDefNode{main}\n"
      "  FuncProtoNode{main}\n"
      "  ScopedNode\n"
      "   StatementNode\n"
      "    DeclarationNode\n"
      "     SymbolNode{a}\n"
      "      TypedNode\n"
      "      InitializerNode\n"
      "       ConstNode{0}\n"
      "   StatementNode\n"
      "    DeclarationNode\n"
      "     SymbolNode{i}\n"
      "      TypedNode\n"
      "      InitializerNode\n"
      "       ConstNode{0}\n"
      "   StatementNode\n"
      "    WhileLoopNode\n"
      "     StatementNode\n"
      "      UnaryOpNode[++]\n"
      "       RefSymbolNode[i]\n"
      "     StatementNode\n"
      "      UnaryOpNode[++]\n"
      "       RefSymbolNode[a]\n"
      "     BinaryOpNode[<]\n"
      "      RefSymbolNode[i]\n"
      "      ConstNode{10}\n"
      ;
  const std::string str =
      " void main() {\n"
      "  int a = 0;  \n"
      "  int i = 0;  \n"
      "  do {\n"
      "    i++;\n"
      "    a++;\n"
      "  } while (i < 10); \n"
      "}\n"
      ;

  azer::afx::ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  azer::afx::ASTNodeFactory factory;
  azer::afx::ParseContext context("", "", str, &factory, opt);
  azer::afx::Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  std::string dump;
  azer::afx::ASTreeDumper dumper;
  dumper.Dump(context.root(), &dump);
  ASSERT_EQ(dump, expect);
  // LOG(ERROR) << dump;
}

TEST(ASTree, SwitchCase) {
  const std::string expect =
      "ScopedNode\n"
      " FuncDefNode{psmain}\n"
      "  FuncProtoNode{psmain}\n"
      "  ScopedNode\n"
      "   ActParamNode{a}\n"
      "    TypedNode\n"
      "   StatementNode\n"
      "    SwitchNode\n"
      "     StatementNode\n"
      "      CaseNode\n"
      "     StatementNode\n"
      "      ReturnNode\n"
      "       FuncCallTypeInitNode{vector4}\n"
      "        ConstNode{1}\n"
      "        ConstNode{1}\n"
      "        ConstNode{1}\n"
      "        ConstNode{1}\n"
      "     StatementNode\n"
      "      CaseNode\n"
      "     StatementNode\n"
      "      ReturnNode\n"
      "       FuncCallTypeInitNode{vector4}\n"
      "        ConstNode{2}\n"
      "        ConstNode{2}\n"
      "        ConstNode{2}\n"
      "        ConstNode{1}\n"
      "     StatementNode\n"
      "      CaseNode\n"
      "     StatementNode\n"
      "      ReturnNode\n"
      "       FuncCallTypeInitNode{vector4}\n"
      "        ConstNode{0}\n"
      "        ConstNode{0}\n"
      "        ConstNode{0}\n"
      "        ConstNode{0}\n"
      ;
  const std::string str =
      " vec4 psmain(int a) {\n"
      "   switch(a) {\n"
      "     case 1: return vec4(1, 1, 1, 1);\n"
      "     case 2: return vec4(2, 2, 2, 1);\n"
      "     default: return vec4(0, 0, 0, 0);\n"
      "   }\n"
      " }\n"
      ;

  azer::afx::ParseContext::Options opt;
  // opt.dump_parser = true;
  // opt.dump_tokenizer = true;
  azer::afx::ASTNodeFactory factory;
  azer::afx::ParseContext context("", "", str, &factory, opt);
  azer::afx::Parser parser;
  ASSERT_TRUE(parser.Parse(&context));

  std::string dump;
  azer::afx::ASTreeDumper dumper;
  dumper.Dump(context.root(), &dump);
  // LOG(ERROR) << dump;
  ASSERT_EQ(dump, expect);
}
