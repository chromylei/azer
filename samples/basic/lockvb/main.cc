#include "azer/render/render.h"
#include "azer/math/math.h"
#include "azer/util/util.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"

#include "diffuse.afx.h"
#include <tchar.h>

#define EFFECT_GEN_DIR "out/dbg/gen/samples/basic/lockvb/"
#define SHADER_NAME "diffuse.afx"
using base::FilePath;

DiffuseEffect::Vertex g_vertices[] = {
  DiffuseEffect::Vertex(azer::Vector4( 0.0f, 1.0f, 0.5f, 1.0f ),
                        azer::Vector4( 1.0f, 0.0f, 0.0f, 1.0f )),
  DiffuseEffect::Vertex(azer::Vector4( -1.0f, -1.0f, 0.5f, 1.0f ),
                        azer::Vector4( 0.0f, 1.0f,  0.0f, 1.0f )),
  DiffuseEffect::Vertex(azer::Vector4( 1.0f, -1.0f, 0.5f, 1.0f ),
                        azer::Vector4( 0.0f, 0.0f, 1.0f, 1.0f )),
};

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

    data_.reset(new azer::VertexData(effect_->GetVertexDesc(),
                                     ARRAYSIZE(g_vertices)));
    memcpy(data_->pointer(), (uint8*)g_vertices, sizeof(g_vertices));
    azer::VertexBuffer::Options vbopt;
    vbopt.usage = azer::GraphicBuffer::kDynamic;
    vbopt.cpu_access = azer::kCPUWrite;
    vb_.reset(rs->CreateVertexBuffer(vbopt, data_));
    renderer->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
    CHECK(renderer->GetFrontFace() == azer::kCounterClockwise);
    CHECK(renderer->GetCullingMode() == azer::kCullBack);
  }
  virtual void OnUpdateScene(double time, float delta_time) {
    static double prev_time = time;
    static int cnt = 0;
    
    static azer::Vector4 diffuse[] = {
      azer::Vector4( 1.0f, 0.0f, 0.0f, 1.0f ),
      azer::Vector4( 0.0f, 1.0f, 0.0f, 1.0f ),
      azer::Vector4( 0.0f, 0.0f, 1.0f, 1.0f ),
    };
    if (time - prev_time) {
      prev_time = time;
      azer::LockDataPtr dataptr(vb_->map(azer::kWriteDiscard));
      uint8* data = (uint8*)dataptr->data_ptr();
      for (int i = 0; i < arraysize(g_vertices); ++i) {
        g_vertices[i].diffuse = diffuse[(cnt + i) % arraysize(diffuse)];
      }
      memcpy(data, g_vertices, sizeof(g_vertices));
      vb_->unmap();
      cnt++;
    }
  }

  virtual void OnRenderScene(double time, float delta_time) {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::Renderer* renderer = rs->GetDefaultRenderer();
    DCHECK(NULL != rs);
    renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    effect_->Use(renderer);
    renderer->Render(vb_.get(), azer::kTriangleList, 3, 0);
  }

  virtual void OnQuit() {}
 private:
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  std::unique_ptr<DiffuseEffect> effect_;
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


