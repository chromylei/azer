#include "sandbox/ui/surface/delegate.h"
#include "azer/render/render.h"
#include "azer/ui/gfx/gfx.h"
#include "azer/ui/widget/widget_all.h"


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

  // gfx::RectF rect = gfx::RectF(0.0f, 0.0f, 0.4f, 0.3f);
  gfx::Rect win(0, 0, 800, 600);
  gfx::Rect rc1(400, 0, 400, 300);
  gfx::RectF rect = azer::WindowToRenderer(rc1, win);
  surface_.reset(rs->CreateSurface(rect));

  camera_.SetPosition(azer::Vector3(0, 150, 30));
  camera_.SetTarget(azer::Vector3(150, 150, 150));

  int width = 150, height = 100;
  std::shared_ptr<gfx::Canvas> canvas(gfx::Canvas::create(width, height));
  canvas->clear(SkColorSetARGB(0, 0, 0, 0));
  canvas->FillRect(gfx::Rect(0, 0, width, height), SkColorSetARGB(0, 0, 0, 0),
                   SkXfermode::kClear_Mode);
  canvas->drawText("Hello, gfx", 0, 20);
  canvas->drawText("Hello, surface", 0, 40);
  tex_.reset(canvas->CreateTexture(rs));
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  azer::Matrix4 scale = azer::Scale(10.0f, 10.0f, 10.0f);
  gridworld_ = scale;
  camera_.OnInput();
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  rs->Clear(azer::Vector4(0.1f, 0.1f, 0.1f, 1.0f));
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  rs->ClearDepthAndStencil();

  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, kWindowWidth, kWindowHeight));
  sphere_.Render(camera_);
  grid_.Render(rs, gridworld_, &camera_, light_);

  rs->SetCullingMode(azer::kCullNone);
  surface_->Draw(tex_.get(), rs);
  
  rs->Present();
}

void MainDelegate::InitLight() {
  light_.set_directional(azer::vec4(0.25f, -0.5f, -1.0f, 0.0f));
  light_.set_ambient(azer::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  light_.set_diffuse(azer::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

