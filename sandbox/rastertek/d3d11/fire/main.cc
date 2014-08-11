#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"

#include "azer/util/util.h"
#include "sandbox/rastertek/d3d11/common/load.h"
#include "afx_fire_effect.h"
#include <tchar.h>

#define EFFECT_GEN_DIR "out/dbg/gen/sandbox/rastertek/d3d11/fire/"
#define SHADER_NAME "afx_fire_effect"

using azer::VertexBuffer;
using azer::VertexData;
using base::FilePath;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate()
      : camera_(azer::vec4(0, 0, 8, 1),
                azer::vec4(0, 0, 0, 1),
                azer::vec4(0, 1, 0, 0))
                {}
  virtual void OnCreate() {}

  void Init();
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);
  virtual void OnQuit() {}
 private:
  std::unique_ptr<FireEffect> effect_;

  azer::VertexDataPtr  square_data_;
  azer::VertexBufferPtr square_vb_;
  azer::TexturePtr alpha_tex_;
  azer::TexturePtr fire_tex_;
  azer::TexturePtr noise_tex_;
  azer::Matrix4 square_world_;
  
  azer::Matrix4 projection_;
  azer::Camera camera_;
  azer::BlendingPtr blending_;
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

void MainDelegate::Init() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
    
  effect_.reset(new FireEffect(shaders.GetShaderVec(), rs));
  {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/square.txt"))));
    square_data_.reset(new VertexData(effect_->GetVertexDesc(),
                                    vertices.size()));
    memcpy(square_data_->pointer(), (uint8*)&vertices[0],
           sizeof(FireEffect::Vertex) * vertices.size());
    square_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), square_data_));
    fire_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/fire/fire01.dds"))));
    alpha_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/fire/alpha01.dds"))));
    noise_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/fire/noise01.dds"))));
  }

  rs->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
  rs->SetFrontFace(azer::kCounterClockwise);
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  rs->SetCullingMode(azer::kCullNone);
  projection_ = azer::PerspectiveRHD3D(azer::Degree(45.0f),
                                       4.0f / 3.0f,
                                       0.1f, 1000.0f);
  square_world_ = azer::Translate(0.0, 0.0, 2.0);
  {
    using azer::Blending;
    azer::Blending::Desc desc;
    desc.src = Blending::kSrcColor;
    desc.dest = Blending::kDestColor;
    desc.oper = Blending::kAdd;
    desc.src_alpha = Blending::kSrcAlpha;
    desc.dest_alpha = Blending::kSrcInvAlpha;
    desc.alpha_oper = Blending::kAdd;
    desc.mask = Blending::kWriteAll;
    blending_.reset(rs->CreateBlending(desc));
  }
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  float rspeed = 3.14f * 2.0f / 4.0f;
  const float speed = 2.0f;
  if( ::GetAsyncKeyState('A') & 0x8000f ) {
    camera_.strafe(-speed * delta_time);
  }
  if( ::GetAsyncKeyState('D') & 0x8000f ) {
    camera_.strafe(speed * delta_time);
  }
  if( ::GetAsyncKeyState('W') & 0x8000f ) {
    camera_.walk(+speed * delta_time);
  }
  if( ::GetAsyncKeyState('S') & 0x8000f ) {
    camera_.walk(-speed * delta_time);
  }
  if( ::GetAsyncKeyState('H') & 0x8000f ) {
    camera_.fly(+speed * delta_time);
  }
  if( ::GetAsyncKeyState('J') & 0x8000f ) {
    camera_.fly(-speed * delta_time);
  }
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  
  rs->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  rs->ClearDepthAndStencil();
  rs->SetCullingMode(azer::kCullNone);
  rs->ResetBlending();
  

  float blend_factor[] = {0.75f, 0.75f, 0.75f, 1.0f};
  blending_->Use(blend_factor);
  azer::mat4 wvp = projection_ * camera_.GenViewMat() * square_world_;
  effect_->set_vs_wvp(wvp);
  effect_->set_vs_world(square_world_);
  effect_->set_vs_deltatime((float)time);
  effect_->set_vs_scroll_speed(azer::Vector4(1.3f, 2.1f, 2.3f, 1.0f));
  effect_->set_ps_FireTex(fire_tex_.get());
  effect_->set_ps_AlphaTex(alpha_tex_.get());
  effect_->set_ps_NoiseTex(noise_tex_.get());
  effect_->set_ps_distortion0(azer::vec4(0.1f, 0.2f, 1.0f, 1.0f));
  effect_->set_ps_distortion1(azer::vec4(0.1f, 0.1f, 1.0f, 1.0f));
  effect_->set_ps_distortion2(azer::vec4(0.1f, 0.3f, 1.0f, 1.0f));
  effect_->set_ps_distortion_scale(0.8f);
  effect_->set_ps_distortion_bias(0.5f);
  effect_->Use(rs);
  rs->Render(square_vb_.get(), azer::kTriangleList);
}
