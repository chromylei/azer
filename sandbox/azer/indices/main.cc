#include "azer/render/render.h"
#include "sandbox/azer/common/config.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"
#include <tchar.h>

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  void Init();
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time) {}
  virtual void OnRenderScene(double time, float delta_time);

  virtual void OnQuit() {}
 private:
  std::unique_ptr<azer::Technique> tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  azer::IndicesBufferPtr ib_;

  std::string vertex_shader_;
  std::string fragment_shader_;

  static const azer::VertexDesc::Desc kVertexDescs[];
};

struct Vertex {
  azer::Vector4 position;
  azer::Vector4 color;

  Vertex(const azer::Vector4& p, const azer::Vector4& c)
      : position(p), color(c) {
  }
};

Vertex v[] = {
  Vertex(azer::Vector4(-0.5f, -0.5f, 0.5f, 1.0f),
         azer::Vector4(1.0f, 0.0f, 0.0f, 1.0f )),
  Vertex(azer::Vector4(-0.5f,  0.5f, 0.5f, 1.0f),
         azer::Vector4(0.0f, 1.0f, 0.0f, 1.0f )),
  Vertex(azer::Vector4( 0.5f,  0.5f, 0.5f, 1.0f),
         azer::Vector4(0.0f, 0.0f, 1.0f, 1.0f )),
  Vertex(azer::Vector4( 0.5f, -0.5f, 0.5f, 1.0f),
         azer::Vector4(0.0f, 1.0f, 0.0f, 1.0f )),
};

using azer::VertexDesc;
const VertexDesc::Desc MainDelegate::kVertexDescs[] = {
  {"position", VertexDesc::kPosition, azer::kVec4},
  {"color", VertexDesc::kDiffuse, azer::kVec4},
};

::base::FilePath vertex_shader_path(TEXT("sandbox/azer/indices/vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/indices/ps.hlsl"));

void MainDelegate::Init() {
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader_));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader_));
  
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::VertexDescPtr desc(new azer::VertexDesc(kVertexDescs,
                                                ARRAYSIZE(kVertexDescs)));

  tech_.reset(rs->CreateEffect());
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, fragment_shader_));
  vs_.reset(rs->CreateVertexGpuProgram(desc, vertex_shader_));
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);

  tech_->Use();
  data_.reset(new azer::VertexData(desc, ARRAYSIZE(v)));
  memcpy(data_->pointer(), (uint8*)v, sizeof(v));
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));
  tech_->Finalize();

  DWORD indices[] = {2, 1, 0, 3, 2, 0};
  using azer::IndicesData;
  azer::IndicesDataPtr idata(new IndicesData(arraysize(indices),
                                             IndicesData::kUint32,
                                             IndicesData::kMainMemory));
                                             
  memcpy(idata->pointer(), (uint8*)&indices, sizeof(indices));
  ib_.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), idata));

  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, 800, 600));
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  rs->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    
  tech_->Use();
  rs->ClearDepthAndStencil();
  rs->Render(vb_.get(), ib_.get(), azer::kTriangleList, 6, 0);
  rs->Present();
}

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");

  MainDelegate delegate;
  azer::WindowHost win(azer::WindowHost::Options(), &delegate);
  win.Init();
  CHECK(azer::RenderSystem::InitRenderSystem(
      ::base::FilePath(RENDER_SYSTEM_DLL), &win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  delegate.Init();
  
  // initialize shader
  
  win.Show();

  azer::MainRenderLoop(&win);
  return 0;
}
