#include "sandbox/azer/heightmaps/delegate.h"


MainDelegate::MainDelegate()
    : light_(azer::Light::kDirectionalLight) {
}


void MainDelegate::Init() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  
  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, kWindowWidth, kWindowHeight));

  InitLight();
  sphere_.Init();
  grid_.Init(base::FilePath(TEXT("sandbox/azer/media/heightmap.bmp")));
  camera_.SetPosition(azer::Vector3(0, 150, 30));
  camera_.SetTarget(azer::Vector3(150, 150, 150));

  rs->SetFrontFace(azer::kCounterClockwise);
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
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
  rs->ClearDepthAndStencil();

  sphere_.Render(camera_);
  
  grid_.Render(rs, gridworld_, &camera_, light_);

  
  rs->Present();
}

void MainDelegate::InitLight() {
  light_.set_directional(azer::vec4(0.25f, -0.5f, -1.0f, 0.0f));
  light_.set_ambient(azer::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  light_.set_diffuse(azer::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

