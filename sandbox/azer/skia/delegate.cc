#include "sandbox/azer/rendertarget/delegate.h"
#include "azer/render/render.h"


MainDelegate::MainDelegate()
    : light_(azer::Light::kDirectionalLight) {
}


void MainDelegate::Init() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  
  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, kWindowWidth, kWindowHeight));

  InitLight();
  sphere_.Init();
  grid_.Init(base::FilePath(TEXT("sandbox/azer/media/heightmap.bmp")));
  camera_.SetPosition(azer::Vector3(0, 5, 0));


  azer::Texture::Options opt;
  opt.width = 400;
  opt.height = 300;
  opt.target = (azer::Texture::BindTarget)
      (azer::Texture::kRenderTarget | azer::Texture::kShaderResource);
  render_target_.reset(rs->CreateRenderTarget(opt));

  opt.target = azer::Texture::kDepthStencil;
  opt.format = azer::kDepth24Stencil8;
  depth_buffer_.reset(rs->CreateDepthBuffer(opt));

  azer::Rectangle rect = azer::Rectangle::FromPointDelta(0.0f, 0.0f, 0.4f, 0.3f);
  spirte_.reset(rs->CreateSpirte(rect));

  camera_.SetPosition(azer::Vector3(0, 150, 30));
  camera_.SetTarget(azer::Vector3(150, 150, 150));
}

void MainDelegate::OnUpdateScene(float time, float delta_time) {
  azer::Matrix4 scale = azer::Scale(10.0f, 10.0f, 10.0f);
  gridworld_ = scale;
  camera_.OnInput();
}

void MainDelegate::OnRenderScene(float time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  rs->Clear(azer::Vector4(0.1f, 0.1f, 0.1f, 1.0f));
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  rs->ClearDepthAndStencil();

  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, kWindowWidth, kWindowHeight));
  sphere_.Render(camera_);
  grid_.Render(rs, gridworld_, &camera_, light_);

  {
    rs->SetViewport(azer::RenderSystem::Viewport(0, 0, 400, 300));
    azer::ScopedRenderTarget scoped_render_target(rs);
    rs->SetRenderTarget(1, render_target_.get(), depth_buffer_.get());
    depth_buffer_->Clear();
    render_target_->Clear(azer::Vector4(0.1f, 0.1f, 0.1f, 1.0f));
    sphere_.Render(camera_);
    grid_.Render(rs, gridworld_, &camera_, light_);
  }

  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, kWindowWidth, kWindowHeight));
  rs->SetCullingMode(azer::kCullNone);
  spirte_->Draw(render_target_->GetTexture(), rs);
  
  rs->Present();
}

void MainDelegate::InitLight() {
  light_.set_directional(azer::vec4(0.25f, -0.5f, -1.0f, 0.0f));
  light_.set_ambient(azer::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  light_.set_diffuse(azer::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

