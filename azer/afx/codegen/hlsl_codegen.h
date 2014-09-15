#pragma once

#include <string>

#include "azer/afx/compiler/astnode.h"
#include "azer/afx/codegen/code_generator.h"

namespace azer {
namespace afx {
class HLSLCodeGeneratorFactory : public CodeGeneratorFactory {
 public:
  virtual ~HLSLCodeGeneratorFactory();
  virtual ASTCodeGenerator* CreateCodeGen(ASTNode* node) OVERRIDE;
 private:
  std::vector<ASTCodeGenerator*> allocated_;
};

class AttributesNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  AttributesNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE {return false;}
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  DISALLOW_COPY_AND_ASSIGN(AttributesNodeHLSLCodeGen);
};

class BinaryOpNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  BinaryOpNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {};
 private:
  void GenBasicOperatorCode(std::string* code);
  void GenMatrixMultiply(std::string* code);
  void GenMatrixMultiplyAndAssign(std::string* code);
  bool IsBasicOper();
  DISALLOW_COPY_AND_ASSIGN(BinaryOpNodeHLSLCodeGen);
};

class CaseNodeNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  CaseNodeNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE;
 private:
  DISALLOW_COPY_AND_ASSIGN(CaseNodeNodeHLSLCodeGen);
};

class ConditionalOpNodeNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  ConditionalOpNodeNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  DISALLOW_COPY_AND_ASSIGN(ConditionalOpNodeNodeHLSLCodeGen);
};

class ConstNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  ConstNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  DISALLOW_COPY_AND_ASSIGN(ConstNodeHLSLCodeGen);
};

class DeclarationNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  DeclarationNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE;
 private:
  bool GenCodeForTexture(std::string* code);
  DISALLOW_COPY_AND_ASSIGN(DeclarationNodeHLSLCodeGen);
};

class FieldNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  FieldNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  bool GenForDecl(std::string* code);
  bool GenForFieldAccess(std::string* code);
  DISALLOW_COPY_AND_ASSIGN(FieldNodeHLSLCodeGen);
};

class ForLoopNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  ForLoopNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  DISALLOW_COPY_AND_ASSIGN(ForLoopNodeHLSLCodeGen);
};

class FuncCallNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  FuncCallNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  std::string GenCodeForSample();
  std::string GenCodeForTexLoad();
  DISALLOW_COPY_AND_ASSIGN(FuncCallNodeHLSLCodeGen);
};

class FuncCallTypeInitNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  FuncCallTypeInitNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  DISALLOW_COPY_AND_ASSIGN(FuncCallTypeInitNodeHLSLCodeGen);
};

class FuncDefNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  FuncDefNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {};
 private:
  DISALLOW_COPY_AND_ASSIGN(FuncDefNodeHLSLCodeGen);
};

class FuncProtoNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  FuncProtoNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}

  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE;
 private:
  DISALLOW_COPY_AND_ASSIGN(FuncProtoNodeHLSLCodeGen);
};

class IfElseNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  IfElseNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  DISALLOW_COPY_AND_ASSIGN(IfElseNodeHLSLCodeGen);
};

class InitializerNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  InitializerNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  DISALLOW_COPY_AND_ASSIGN(InitializerNodeHLSLCodeGen);
};

class JumpNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  JumpNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  JumpNode* node() {
    DCHECK_EQ(ASTCodeGenerator::node()->type(), ASTNode::kJumpNode);
    return (JumpNode*)(ASTCodeGenerator::node());
  }
  DISALLOW_COPY_AND_ASSIGN(JumpNodeHLSLCodeGen);
};

class NullHLSLCodeGen : public ASTCodeGenerator {
 public:
  NullHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE {return false;}
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  DISALLOW_COPY_AND_ASSIGN(NullHLSLCodeGen);
};

class RefSymbolHLSLCodeGen : public ASTCodeGenerator {
 public:
  RefSymbolHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {}
 private:
  DISALLOW_COPY_AND_ASSIGN(RefSymbolHLSLCodeGen);
};

class ReturnHLSLCodeGen : public ASTCodeGenerator {
 public:
  ReturnHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {};
 private:
  DISALLOW_COPY_AND_ASSIGN(ReturnHLSLCodeGen);
};

class ScopedNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  ScopedNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE;
 private:
  DISALLOW_COPY_AND_ASSIGN(ScopedNodeHLSLCodeGen);
};

class StatementNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  StatementNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE;
 private:
  DISALLOW_COPY_AND_ASSIGN(StatementNodeHLSLCodeGen);
};

class StructDeclNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  StructDeclNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE;
 private:
  DISALLOW_COPY_AND_ASSIGN(StructDeclNodeHLSLCodeGen);
};

class SwitchNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  SwitchNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {};
 private:
  SwitchNode* node() {
    DCHECK_EQ(ASTCodeGenerator::node()->type(), ASTNode::kSwitchNode);
    return (SwitchNode*)(ASTCodeGenerator::node());
  }
  DISALLOW_COPY_AND_ASSIGN(SwitchNodeHLSLCodeGen);
};

class SymbolNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  SymbolNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {};
 private:
  // bool GenCodeForTexture(SymbolNode* symbol, std::string* code);
  bool GenCommonCode(SymbolNode* symbol, std::string* code);
  DISALLOW_COPY_AND_ASSIGN(SymbolNodeHLSLCodeGen);
};

class ActParamNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  ActParamNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {};
 private:
  DISALLOW_COPY_AND_ASSIGN(ActParamNodeHLSLCodeGen);
};

class UnaryOpNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  UnaryOpNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE;
 private:
  UnaryOpNode* node() {
    DCHECK_EQ(ASTCodeGenerator::node()->type(), ASTNode::kUnaryOpNode);
    return (UnaryOpNode*)(ASTCodeGenerator::node());
  }
  DISALLOW_COPY_AND_ASSIGN(UnaryOpNodeHLSLCodeGen);
};

class WhileLoopNodeHLSLCodeGen : public ASTCodeGenerator {
 public:
  WhileLoopNodeHLSLCodeGen(ASTNode* node) : ASTCodeGenerator(node) {}
  
  virtual bool GenCodeBegin(std::string* code) OVERRIDE;
  virtual void GenCodeEnd(std::string* code) OVERRIDE {};
 private:
  DISALLOW_COPY_AND_ASSIGN(WhileLoopNodeHLSLCodeGen);
};
}  // namespace afx
}  // namespace azer
