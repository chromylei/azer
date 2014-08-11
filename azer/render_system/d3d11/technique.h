#pragma once

#include "base/basictypes.h"
#include "azer/render/technique.h"
#include "azer/render/gpu_program.h"

namespace azer {

class D3D11RenderSystem;

class D3D11Technique : public Technique {
 public:
  D3D11Technique(D3D11RenderSystem* rs)
      : render_system_(rs) {
  }

  virtual bool Finalize() {
    return true;
  }

  virtual void Use(Renderer*);
 private:
  D3D11RenderSystem* render_system_;
  DISALLOW_COPY_AND_ASSIGN(D3D11Technique);
};
}  // namespace azer
