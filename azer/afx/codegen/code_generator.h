#pragma once

#include "base/basictypes.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/compiler/context.h"

namespace azer {
namespace afx {

class SnippetCodeGenerator;
class ASTCodeGenerator {
 public:
  ASTCodeGenerator(ASTNode* node) : node_(node) {}
  virtual ~ASTCodeGenerator() {}

  virtual bool GenCodeBegin(std::string* code) = 0;
  virtual void GenCodeEnd(std::string* code) = 0;
  ASTNode* node() { return node_;}
 private:
  ASTNode* node_;
  DISALLOW_COPY_AND_ASSIGN(ASTCodeGenerator);
};

class CodeGeneratorFactory {
 public:
  virtual ~CodeGeneratorFactory() {}
  virtual ASTCodeGenerator* CreateCodeGen(ASTNode* node) = 0;
};

class SnippetCodeGenerator : public TreeNode<ASTNode>::Traverser {
 public:
  SnippetCodeGenerator(CodeGeneratorFactory* factory)
      : factory_(factory) {
  }
  void GenCode(ASTNode* node);
  const std::string& GetCode() const { return code_gen_;}
 private:
  virtual bool OnVisitBegin(ASTNode* node) OVERRIDE;
  virtual void OnVisitEnd(ASTNode* node) OVERRIDE;
  std::string code_gen_;
  CodeGeneratorFactory* factory_;

  DISALLOW_COPY_AND_ASSIGN(SnippetCodeGenerator);
};

class CodeGenerator {
 public:
  CodeGenerator(CodeGeneratorFactory* factory)
      : generator_(factory) {
  }

  void GenCode(ParseContext* context) {
    generator_.GenCode(context->root());
  }
 private:
  SnippetCodeGenerator generator_;
  CodeGeneratorFactory* factory_;
  DISALLOW_COPY_AND_ASSIGN(CodeGenerator);
};
}  // namespace afx
}  // namespace azer
