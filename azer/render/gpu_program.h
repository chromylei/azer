#pragma once

#include <memory>
#include <string>
#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/base/render_export.h"
#include "azer/base/string.h"
#include "azer/render/vertex_data.h"
#include "azer/render/render_system_enum.h"

namespace azer {

class RenderSystem;

enum ShaderType {
  kStringShader,
  kBinaryShader,
};

class AZER_EXPORT GpuProgram {
 public:
  virtual ~GpuProgram() {}

  std::string error_msg() const { return error_msg_;}
  RenderPipelineStage stage() const { return stage_;}

  virtual bool Init(RenderSystem* rs) = 0;

  /**
   * 每一个 shader 都有对应的输入输出格式，通过以下接口可以检查
   * Render Pipeline 的过程当中各阶段输入输出是否匹配
   */
  virtual VertexDescPtr GetInputDesc() { return NULL;}
  virtual VertexDescPtr GetOutputDesc() { return NULL;}
 protected:
  GpuProgram(RenderPipelineStage stage, const std::string& program)
      : stage_(stage)
      , program_(program) {}

  RenderPipelineStage stage_;
  std::string program_;
  std::string error_msg_;
  DISALLOW_COPY_AND_ASSIGN(GpuProgram);
};

class AZER_EXPORT VertexGpuProgram : public GpuProgram {
 public:
  virtual VertexDescPtr GetInputDesc() { return desc_ptr_;}
 protected:
  VertexGpuProgram(VertexDescPtr& desc, const std::string& program)
      : GpuProgram(kVertexStage, program)
      , desc_ptr_(desc) {
  }

  VertexDescPtr desc_ptr_;
  DISALLOW_COPY_AND_ASSIGN(VertexGpuProgram);
};

inline CharType* ShaderSuffix(ShaderType type) {
  switch (type) {
    case kBinaryShader: return AZER_LITERAL("bin");
    case kStringShader: return AZER_LITERAL("str");
    default: NOTREACHED(); return AZER_LITERAL("");
  }
}

typedef std::shared_ptr<GpuProgram> GpuProgramPtr;
typedef std::shared_ptr<VertexGpuProgram> VertexGpuProgramPtr;
}  // namespace azer
