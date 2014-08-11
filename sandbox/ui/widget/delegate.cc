#include "sandbox/ui/widget/delegate.h"
#include "azer/render/render.h"
#include "azer/ui/widget/widget_all.h"
#include "base/strings/utf_string_conversions.h"


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

  camera_.SetPosition(azer::Vector3(0, 150, 30));
  camera_.SetTarget(azer::Vector3(150, 150, 150));

  root_ui_.reset(azer::RootWindow::Create(rs));

  {
    gfx::Rect rc(0, 0, 400, 300);
    gfx::Rect texrc(10, 20, 300, 30);
    azer::Window* win = azer::Window::Create(rc, root_ui_.get());
    fps_label_ = azer::Label::Create(texrc, win);
  }
  
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  azer::Matrix4 scale = azer::Scale(10.0f, 10.0f, 10.0f);
  gridworld_ = scale;
  camera_.OnInput();
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  std::stringstream ss;
  // ss << "FPS: " << 1.0f / delta_time;
  counter_.Update(time, delta_time);
  static double prevtime = time - 3.0;
  if (time - prevtime > 2.0) {
    prevtime = time;
    ss << "FPS: " << counter_.GetAverageFPS(time, delta_time)
       << ", Total Frames: " << counter_.Frames();
    fps_label_->SetText(base::UTF8ToUTF16(ss.str()));
    LOG(ERROR) << ss.str();
    root_ui_->Redraw();
  }
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
  
  root_ui_->Render();
  rs->Present();
}

void MainDelegate::InitLight() {
  light_.set_directional(azer::vec4(0.25f, -0.5f, -1.0f, 0.0f));
  light_.set_ambient(azer::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  light_.set_diffuse(azer::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

