#pragma once

#include <string>

#include "base/basictypes.h"
#include "azer/afx/compiler/astnode.h"
#include "azer/afx/linker/technique_parser.h"

namespace azer {
namespace afx {

/**
 * CppCodeGen
 * 生成 C++ 相关的代码
 */
class CppCodeGen {
 public:
  CppCodeGen();
  ~CppCodeGen();

  void GenCode(const TechniqueParser::Technique& tech);
  const std::string& GetHeadCode() const { return head_code_;}
  const std::string& GetCppCode() const { return cpp_code_;}
 private:
  std::string GetClassName(const TechniqueParser::Technique& tech) const;

  /**
   * 生成制定 technique 的相关 c++ 代码
   */
  std::string GenTechnique(const TechniqueParser::Technique& tech);
  void GenHeadCode(const TechniqueParser::Technique& tech);
  void GenCppCode(const TechniqueParser::Technique& tech);

  std::string GenSetMaterial(const TechniqueParser::Technique& tech);
  std::string GenInit(const TechniqueParser::Technique& tech);
  std::string GenGpuTableInit(const TechniqueParser::Technique& tech);
  std::string GenVertexStruct(const TechniqueParser::Technique& tech);
  std::string GenVertexDesc(const TechniqueParser::Technique& tech);

  // generate structure of uniform variables
  std::string GenExchangeBuffer(const TechniqueParser::Technique& tech) const;
  std::string GenStageExchangeBuffer(RenderPipelineStage stage,
                                     const ASTNodeVec& uniforms) const;
  std::string GenStageGpuTable(RenderPipelineStage stage,
                               const ASTNodeVec& uniforms);
  std::string GenStageGpuTableDesc(RenderPipelineStage stage,
                                   const ASTNodeVec& uniforms);
  std::string GenStageUniformStructure(RenderPipelineStage stage,
                                       const ASTNodeVec& uniforms) const;

  // generate function to set uniform variables
  std::string GenUniformFuncs(const TechniqueParser::Technique& tech);
  std::string GenStageUniforms(RenderPipelineStage stage, const ASTNodeVec& unif);
  std::string GenStageTexture(RenderPipelineStage stage, const ASTNodeVec& unif);
  std::string GenAllTextureMember(const TechniqueParser::Technique& tech);
  std::string GenStageTextureMember(RenderPipelineStage stage,
                                    const ASTNodeVec& uniforms);

  /**
   * 生成 Effect 的虚函数 UseTexture
   */
  std::string GenUseTexture(const TechniqueParser::Technique& tech);
  std::string GenTextureStageUse(RenderPipelineStage stage,
                                 const ASTNodeVec& uniforms);
  std::string cpp_code_;
  std::string head_code_;
  DISALLOW_COPY_AND_ASSIGN(CppCodeGen);
};
}  // namespace afx
}  // namespace azer
