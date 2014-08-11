#pragma once

#include <string>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/render/gpu_program.h"
#include "azer/render/vertex_data.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {

class D3D11RenderSystem;
class D3D11VertexGpuProgram : public VertexGpuProgram {
 public:
  D3D11VertexGpuProgram(VertexDescPtr desc, const std::string& program)
      : VertexGpuProgram(desc, program)
      , shader_(NULL)
      , input_layout_(NULL) {
  }

  virtual ~D3D11VertexGpuProgram() {
    SAFE_RELEASE(shader_);
    SAFE_RELEASE(input_layout_);
  }

  virtual bool Init(RenderSystem* rs) OVERRIDE;
 private:
  ID3D11VertexShader* shader_;
  ID3D11InputLayout *input_layout_;

  friend class D3D11Renderer;
  friend class D3D11Technique;
  DISALLOW_COPY_AND_ASSIGN(D3D11VertexGpuProgram);
};

class D3D11PixelGpuProgram : public GpuProgram {
 public:
  D3D11PixelGpuProgram(const std::string& program)
      : GpuProgram(kPixelStage, program)
      , shader_(NULL) {
  }

  virtual ~D3D11PixelGpuProgram() {
    SAFE_RELEASE(shader_);
  }

  virtual bool Init(RenderSystem* rs) OVERRIDE;
 private:
  ID3D11PixelShader* shader_;
  friend class D3D11Renderer;
  friend class D3D11Technique;
  DISALLOW_COPY_AND_ASSIGN(D3D11PixelGpuProgram);
};

class D3D11GeometryGpuProgram : public GpuProgram {
 public:
  D3D11GeometryGpuProgram(const std::string& program)
      : GpuProgram(kGeometryStage, program)
      , shader_(NULL) {
  }

  virtual ~D3D11GeometryGpuProgram() {
    SAFE_RELEASE(shader_);
  }

  virtual bool Init(RenderSystem* rs) OVERRIDE;
 private:
  ID3D11GeometryShader* shader_;

  friend class D3D11Renderer;
  friend class D3D11Technique;
  DISALLOW_COPY_AND_ASSIGN(D3D11GeometryGpuProgram);
};
}  // namespace azer
