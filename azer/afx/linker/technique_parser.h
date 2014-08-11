#pragma once

#include <string>
#include <vector>

#include "azer/afx/compiler/astnode.h"
#include "azer/render/render_system_enum.h"

namespace azer {
namespace afx {
class ParseContext;

class TechniqueParser {
 public:
  TechniqueParser();
  ~TechniqueParser();

  struct StageInfo {
    ASTNode* entry;
    ASTNodeVec depend;
    ASTNodeVec uniforms;
    ASTNodeVec uni_depend;
    ASTNodeVec uni_textures;
    StageInfo(): entry(NULL) {}
  };

  struct Technique {
    std::string name;
    std::vector<StageInfo> shader;
    AttributesNode* attributes;
  };

  /**
   * 根据 attributes 的定义找到入口点并由入口点开始生成代码
   * 首先生成的倚赖的代码
   **/
  bool Parse(ParseContext* context);
  typedef std::map<std::string, Technique> TechniqueDict;
  bool GetTechnique(const std::string& name, Technique** technique);
  const TechniqueDict& GetTechniques() const { return techniques_;}
 private:
  void CalcFuncDeps(Technique* technique);
  void ReportError(const std::string& errtext);
  bool GenTechnique(AttributesNode* node, ParseContext* context);

  /**
   * 检查 Technique 是否合法
   * 1. 检查各个阶段主函数的参数输入如果是结构体，那么它是否是 exchange 类型的
   * Technique 的各个属性值是否合法
   */
  // TODO
  bool Valid() { return true;}
  ASTNodeVec pipeline_;
  TechniqueDict techniques_;
  std::string error_text_;
  DISALLOW_COPY_AND_ASSIGN(TechniqueParser);
};
}  // namespace afx
}  // namespace azer
