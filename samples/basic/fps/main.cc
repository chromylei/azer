#include "azer/render/render.h"
#include "azer/math/math.h"
#include "azer/util/util.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "azer/ui/widget/widget_all.h"
#include "azer/render/util/frame_counter.h"
#include "base/strings/utf_string_conversions.h"

#include "diffuse.h"
#include <tchar.h>

#define EFFECT_GEN_DIR "out/dbg/gen/samples/basic/fps/"
#define SHADER_NAME "diffuse"
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


    effect_.reset(new DiffuseEffect(shaders.GetShaderVec(),
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

    root_ui_.reset(azer::RootWindow::Create(rs));
    {
      gfx::Rect rc(0, 0, 400, 300);
      gfx::Rect texrc(10, 20, 300, 30);
      azer::Window* win = azer::Window::Create(rc, root_ui_.get());
      fps_label_ = azer::Label::Create(texrc, win);
    }
  }
  virtual void OnUpdateScene(double time, float delta_time) {
    counter_.Update(time, delta_time);
    static double prevtime = time - 3.0;
    if (time - prevtime > 2.0) {
      prevtime = time;
      std::stringstream ss;
      ss << "FPS: " << counter_.GetAverageFPS(time, delta_time)
         << ", Total Frames: " << counter_.Frames();
      fps_label_->SetText(base::UTF8ToUTF16(ss.str()));
      LOG(ERROR) << ss.str();
      root_ui_->Redraw();
    }
  }

  virtual void OnRenderScene(double time, float delta_time) {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::Renderer* renderer = rs->GetDefaultRenderer();
    DCHECK(NULL != rs);
    renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    effect_->set_vs_diffuse(azer::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    effect_->Use(renderer);
    renderer->Render(vb_.get(), azer::kTriangleList, 3, 0);

    root_ui_->Render();
  }

  virtual void OnQuit() {}
 private:
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  std::unique_ptr<DiffuseEffect> effect_;
  std::unique_ptr<azer::RootWindow> root_ui_;
  azer::Label* fps_label_;
  azer::FrameCounter counter_;
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


