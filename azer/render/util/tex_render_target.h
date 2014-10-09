#pragma once

#include <memory>

#include "azer/render/render_system.h"
#include "azer/render/renderer.h"
#include "azer/base/render_export.h"

namespace azer {
class TexRenderTarget {
 public:
  explicit TexRenderTarget(azer::Texture::Options& opt)
      : options_(opt) {
  }

  TexRenderTarget(int width, int height) {
    options_.width = width;
    options_.height = height;
    options_.format = azer::kRGBAf;
    options_.target = (azer::Texture::BindTarget)
        (azer::Texture::kRenderTarget | azer::Texture::kShaderResource);
  }

  void Init(azer::RenderSystem* rs);
  azer::Renderer* Begin(const azer::Vector4& color);
  void Reset(azer::Renderer* renderer);

  azer::TexturePtr& GetRTTex() {
    return renderer_->GetRenderTarget()->GetTexture();
  }

  azer::Renderer* GetRenderer() { return renderer_.get();}
 private:
  std::unique_ptr<azer::Renderer> renderer_;
  azer::Texture::Options options_;
  DISALLOW_COPY_AND_ASSIGN(TexRenderTarget);
};

inline void TexRenderTarget::Init(azer::RenderSystem* rs) {
  renderer_.reset(rs->CreateRenderer(options_));
  renderer_->SetViewport(Renderer::Viewport(0, 0, options_.width, options_.height));
  renderer_->EnableDepthTest(true);
}

inline azer::Renderer* TexRenderTarget::Begin(const azer::Vector4& color) {
  renderer_->Use();
  renderer_->SetViewport(Renderer::Viewport(0, 0, options_.width, options_.height));
  renderer_->Clear(color);
  renderer_->ClearDepthAndStencil();
  
  return renderer_.get();
}

inline void TexRenderTarget::Reset(azer::Renderer* renderer) {
  renderer->Use();
}


typedef std::shared_ptr<TexRenderTarget> TexRenderTargetPtr;
}  // namespace azer
