#pragma once

#include <d3d11.h>

#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/render/gpu_constants_table.h"
#include "azer/render_system/d3d11/util.h"

namespace azer {
class D3D11Renderer;
class D3D11RenderSystem;

class D3D11GpuConstantsTable : public GpuConstantsTable {
 public:
  virtual ~D3D11GpuConstantsTable() {
    SAFE_RELEASE(buffer_);
  }

  virtual void flush(Renderer*) OVERRIDE;
 private:
  D3D11GpuConstantsTable(int32 num, const Desc* desc)
      : GpuConstantsTable(num, desc)
      , buffer_(NULL) {
  }
  
 private:
  ID3D11Buffer* buffer_;
  bool Init(D3D11RenderSystem* rs);

  friend class D3D11RenderSystem;
  friend class D3D11Renderer;
  DISALLOW_COPY_AND_ASSIGN(D3D11GpuConstantsTable);
};
}  // namespace azer
