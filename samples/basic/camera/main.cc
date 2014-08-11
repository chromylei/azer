#include "azer/render/render.h"
#include "azer/math/math.h"
#include "azer/util/util.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"

#include "samples/common/common.h"
#include "samples/common/camera_control.h"


#include "camera.h"
#include <tchar.h>

#define EFFECT_GEN_DIR "out/dbg/gen/samples/basic/camera/"
#define SHADER_NAME "camera"
using base::FilePath;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  virtual void OnCreate() {}

  void Init() {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::Renderer* renderer = rs->GetDefaultRenderer();
    azer::ShaderArray shaders;
    CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
    CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));


    effect_.reset(new CameraEffect(shaders.GetShaderVec(),
                                    azer::RenderSystem::Current()));

    azer::Vector4 v[] = {
      azer::Vector4( 0.0f, 1.0f, 0.5f, 1.0f ),
      azer::Vector4( -1.0f, -1.0f, 0.5f, 1.0f ),
      azer::Vector4( 1.0f, -1.0f, 0.5f, 1.0f ),
      
    };
    data_.reset(new azer::VertexData(effect_->GetVertexDesc(), ARRAYSIZE(v)));
    memcpy(data_->pointer(), (uint8*)v, sizeof(v));
    vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));
    renderer->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
    CHECK(renderer->GetFrontFace() == azer::kCounterClockwise);
    CHECK(renderer->GetCullingMode() == azer::kCullBack);

    camera_.SetPosition(azer::Vector3(0.0f, 0.0f, 3.0f));
  }
  virtual void OnUpdateScene(double time, float delta_time) {
    float rspeed = 3.14f * 2.0f / 4.0f;
    azer::Radians camera_speed(azer::kPI / 2.0f);
    UpdatedownCamera(&camera_, camera_speed, delta_time);
  }

  virtual void OnRenderScene(double time, float delta_time) {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::Renderer* renderer = rs->GetDefaultRenderer();
    DCHECK(NULL != rs);
    renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    effect_->SetDiffuse(azer::Vector4(0.0f, 0.0f, 1.0f, 1.0f));

    effect_->SetPVW(camera_.GetProjViewMatrix());
    effect_->Use(renderer);
    renderer->Render(vb_.get(), azer::kTriangleList, 3, 0);
  }

  virtual void OnQuit() {}
 private:
  azer::Camera camera_;
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  std::unique_ptr<CameraEffect> effect_;
};

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  
  MainDelegate delegate;
  azer::WindowHost win(azer::WindowHost::Options(), &delegate);
  win.Init();
  CHECK(azer::LoadRenderSystem(&win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  delegate.Init();
  win.Show();
  azer::MainRenderLoop(&win);
  return 0;
}


