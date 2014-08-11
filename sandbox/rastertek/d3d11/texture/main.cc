#include "azer/render/render.h"
#include "azer/render/util/frame_counter.h"
#include "azer/ui/widget/widget_all.h"
#include "base/logging.h"
#include "sandbox/azer/common/config.h"
#include "base/files/file_path.h"
#include "base/file_util.h"
#include "base/base.h"
#include "samples/common/shader_loader.h"
#include "texture.h"

using base::FilePath;
const std::string shader_path = "sandbox/rastertek/d3d11/texture/";
const std::string shader_name = "texture";

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  void Init();
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time) {}
  virtual void OnRenderScene(double time, float delta_time);

  virtual void OnQuit() {}
 private:
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  azer::TexturePtr tex_;
  std::unique_ptr<TextureEffect> effect_;
  azer::Camera camera_;
};

static const int kWindowWidth = 800;
static const int kWindowHeight = 600;

TextureEffect::Vertex v[] = {
  TextureEffect::Vertex(azer::Vector4(0.0f, 0.5f, 0.5f, 1.0f),
                        azer::Vector2(0.5f, 0.0f)),
  TextureEffect::Vertex(azer::Vector4(-0.5f, -0.5f, 0.5f, 1.0f),
                        azer::Vector2(0.0f, 1.0f)),
  TextureEffect::Vertex(azer::Vector4(0.5f, -0.5f, 0.5f, 1.0f),
                        azer::Vector2(1.0f, 1.0f)),
};


void MainDelegate::Init() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  ShaderLoad loader;
  loader.Load(azer::kVertexStage, shader_path + shader_name + ".vs");
  loader.Load(azer::kPixelStage, shader_path + shader_name + ".ps");
  effect_.reset(new TextureEffect(loader.GetShaderVec(), rs));
  data_.reset(new azer::VertexData(effect_->GetVertexDesc(), ARRAYSIZE(v)));
  memcpy(data_->pointer(), (uint8*)v, sizeof(v));
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));

  rs->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
  tex_.reset(rs->CreateTextureFromFile(
      azer::Texture::k2D, FilePath(TEXT("sandbox/rastertek/media/seafloor.dds"))));
  DCHECK(tex_.get() != NULL);
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  camera_.SetPosition(azer::Vector3(0.0f, 0.0f, 0.0f));
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  rs->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  rs->ClearDepthAndStencil();
  rs->SetCullingMode(azer::kCullNone);
  azer::Matrix4 projection = azer::PerspectiveRHD3D(azer::Degree(45.0f),
                                                    4.0f / 3.0f,
                                                    1.0f, 1000.0f);
  effect_->set_vs_proj(projection * camera_.GenViewMat());
  effect_->set_ps_MyTex(tex_.get());
  effect_->Use(rs);
  rs->Render(vb_.get(), azer::kTriangleList, 3);
}

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  azer::WindowHost::Options opt;
  opt.width = kWindowWidth;
  opt.height = kWindowHeight;
  MainDelegate delegate;
  azer::WindowHost win(opt, &delegate);
  win.Init();
  CHECK(azer::RenderSystem::InitRenderSystem(
      ::base::FilePath(RENDER_SYSTEM_DLL), &win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  delegate.Init();
  
  win.Show();
  azer::MainRenderLoop(&win);
  return 0;
}
