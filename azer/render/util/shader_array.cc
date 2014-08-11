#include "azer/render/util/shader_array.h"
#include "base/file_util.h"
#include "base/strings/utf_string_conversions.h"

namespace azer {
bool LoadVertexShader(const std::string& path, ShaderArray* shaders)  {
  return LoadVertexShader(::base::FilePath(base::UTF8ToWide(path)), shaders);
}

bool LoadVertexShader(const base::FilePath& path, ShaderArray* shaders) {
  return LoadForStage(kVertexStage, path, shaders);
}

bool LoadPixelShader(const std::string& path, ShaderArray* shaders) {
  return LoadPixelShader(::base::FilePath(base::UTF8ToWide(path)), shaders);
}

bool LoadPixelShader(const base::FilePath& path, ShaderArray* shaders) {
  return LoadForStage(kPixelStage, path, shaders);
}

bool LoadForStage(RenderPipelineStage stage, const base::FilePath& path,
                  ShaderArray* shaders) {
  DCHECK(shaders != NULL);
  std::string code;
  if (!::base::ReadFileToString(path, &code)) {
    return false;
  }

  shaders->SetForStage(stage, code);
  return true;
}

TechniquePtr ShaderArray::CreateTechnique(RenderSystem* rs) {
  TechniquePtr tech_ptr(rs->CreateTechnique());
  GpuProgramPtr vertex_ptr(rs->CreateVertexGpuProgram(
      GetVertexDesc(), vec_[(int)kVertexStage]));
  if (vertex_ptr.get()) {
    tech_ptr->AddGpuProgram(vertex_ptr);
  } else {
    return NULL;
  }

  
  GpuProgramPtr pixel_ptr(rs->CreateGpuProgram(kPixelStage, vec_[(int)kPixelStage]));
  if (pixel_ptr.get()) {
    tech_ptr->AddGpuProgram(pixel_ptr);
  } else {
    return NULL;
  }
  
  for (int i = kVertexStage + 1; i < (int)kPixelStage; ++i) {
    if (!vec_[i].empty()) {
      RenderPipelineStage stage = (RenderPipelineStage)i;
      GpuProgramPtr ptr(rs->CreateGpuProgram(stage, vec_[i]));
      tech_ptr->AddGpuProgram(ptr);
    }
  }

  return tech_ptr;
}
}  // namespace azer
