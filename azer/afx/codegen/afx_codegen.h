#pragma once

#include <string>
#include <vector>

#include "azer/afx/linker/technique_parser.h"
#include "base/basictypes.h"

namespace azer {
namespace afx {

class ASTNode;
class CodeGeneratorFactory;

class AfxCodegen {
 public:
  AfxCodegen(CodeGeneratorFactory* factory)
      : factory_(factory)
      , stage_(kStageNotSpec){
  }

  std::string GenCode(RenderPipelineStage stage,
                      const TechniqueParser::StageInfo& shader,
                      bool comments = false);
 private:
  /**
   * 生成 uniforms 类型相关的代码
   * 目前上不支持 glslang 类型的输出
   */
  std::string GenUniform(const std::vector<ASTNode*>& uniforms, bool comments);
  std::string GenTextures(const std::vector<ASTNode*>& textures, bool comments);
  /**
   * 生成指定阶段的 入口函数的代码
   */
  std::string GenEntry(ASTNode* node, bool comments);
  /**
   * 生成依赖项的代码，其中 uniform 类型的数据将被提取出来
   * 用作生成 cbuffer
   */
  std::string GenDeps(const TechniqueParser::StageInfo& shader, bool comments);
  std::string GenDepend(ASTNode* node, bool comments);
  std::string GenUniDepend(ASTNode* node, bool comments);
  std::string GenUniformDeps(const TechniqueParser::StageInfo& shader,
                             bool comments);
  CodeGeneratorFactory* factory_;
  RenderPipelineStage stage_;
  DISALLOW_COPY_AND_ASSIGN(AfxCodegen);
};
}  // namespace afx
}  // namespace azer
