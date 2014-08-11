#pragma once

#include <string>
#include <vector>

#include "base/logging.h"
#include "base/files/file_path.h"
#include "azer/base/render_export.h"
#include "azer/render/technique.h"
#include "azer/render/render_system.h"
#include "azer/render/vertex_data.h"

namespace azer {
class AZER_EXPORT ShaderArray {
 public:
  ShaderArray(ShaderType type = kStringShader);

  void SetForStage(RenderPipelineStage stage, const std::string &code);
  const std::vector<std::string>& GetShaderVec() const { return vec_;}

  VertexDescPtr& GetVertexDesc() { return vertex_desc_;}
  void SetVertexDesc(VertexDescPtr ptr) {vertex_desc_ = ptr;}

  TechniquePtr CreateTechnique(RenderSystem* rs);
 private:
  std::vector<std::string> vec_;
  VertexDescPtr vertex_desc_;
  const ShaderType type_;
  DISALLOW_COPY_AND_ASSIGN(ShaderArray);
};

typedef std::shared_ptr<ShaderArray> ShaderArrayPtr;

inline ShaderArray::ShaderArray(ShaderType type) : type_(type)  {
  vec_.resize(kRenderPipelineStageNum);
}

inline void ShaderArray::SetForStage(RenderPipelineStage stage,
                                     const std::string &code) {
  DCHECK(stage >= kVertexStage && stage <= kPixelStage);
  DCHECK(!code.empty());
  DCHECK(vec_[stage].empty());
  vec_[stage] = code;
}

bool AZER_EXPORT LoadVertexShader(const base::FilePath& path, ShaderArray* shaders);
bool AZER_EXPORT LoadVertexShader(const std::string& path, ShaderArray* shaders);
bool AZER_EXPORT LoadPixelShader(const base::FilePath& path, ShaderArray* shaders);
bool AZER_EXPORT LoadPixelShader(const std::string& path, ShaderArray* shaders);
bool AZER_EXPORT LoadForStage(RenderPipelineStage stage, const base::FilePath& path,
                              ShaderArray* shaders);
}  // namespace azer
