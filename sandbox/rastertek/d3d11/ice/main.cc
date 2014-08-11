#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"

#include "azer/util/util.h"
#include "sandbox/rastertek/d3d11/common/load.h"
#include "afx_ice_effect.h"
#include <tchar.h>

#define EFFECT_GEN_DIR "out/dbg/gen/sandbox/rastertek/d3d11/ice/"
#define SHADER_NAME "afx_ice_effect"

using azer::VertexBuffer;
using azer::VertexData;
using base::FilePath;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate()
      : light_(azer::Light::kDirectionalLight)
      , camera_(azer::vec4(0, 0, 8, 1),
                azer::vec4(0, 0, 0, 1),
                azer::vec4(0, 1, 0, 0))
                {}
  virtual void OnCreate() {}

  void Init();
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);
  virtual void OnQuit() {}
 private:
  std::unique_ptr<IceEffect> effect_;

  azer::VertexDataPtr  cube_data_;
  azer::VertexBufferPtr cube_vb_;
  azer::TexturePtr cube_tex_;
  azer::Matrix4 cube_world_;

  azer::VertexDataPtr  square_data_;
  azer::VertexBufferPtr square_vb_;
  azer::TexturePtr glass_tex_;
  azer::TexturePtr bump_tex_;
  azer::Matrix4 square_world_;
  
  azer::Matrix4 projection_;
  azer::Camera camera_;
 
  azer::Light light_;
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
    
  effect_.reset(new IceEffect(shaders.GetShaderVec(), rs));

  {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/cube.txt"))));
    cube_data_.reset(new VertexData(effect_->GetVertexDesc(),
                                    vertices.size()));
    memcpy(cube_data_->pointer(), (uint8*)&vertices[0],
           sizeof(IceEffect::Vertex) * vertices.size());
    cube_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), cube_data_));
    cube_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/stone01.dds"))));
  }

  {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/square.txt"))));
    square_data_.reset(new VertexData(effect_->GetVertexDesc(),
                                    vertices.size()));
    memcpy(square_data_->pointer(), (uint8*)&vertices[0],
           sizeof(IceEffect::Vertex) * vertices.size());
    square_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), square_data_));
    glass_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/glass01.dds"))));
    bump_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/bump03.dds"))));
  }

  rs->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
  rs->SetFrontFace(azer::kCounterClockwise);
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  rs->SetCullingMode(azer::kCullNone);
  projection_ = azer::PerspectiveRHD3D(azer::Degree(45.0f),
                                       4.0f / 3.0f,
                                       0.1f, 1000.0f);

  light_.set_directional(azer::Vector4(1.0f, -1.0f, -1.0f, 0.0f));
  light_.set_diffuse(azer::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
  light_.set_ambient(azer::Vector4(0.2f, 0.2f, 0.2f, 1.0f));

  cube_tex_.reset(rs->CreateTextureFromFile(
      azer::Texture::k2D,
      base::FilePath(TEXT("sandbox/rastertek/media/seafloor.dds"))));

  square_world_ = azer::Translate(0.0, 1.0, 2.0);
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  float rspeed = 3.14f * 2.0f / 4.0f;
  const float speed = 2.0f;
  cube_world_ = azer::Translate(0.0, 1.0, 0.0)
      * azer::RotateY(azer::Radians(time * rspeed));
  
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

  azer::mat4 wvp = projection_ * camera_.GenViewMat() * cube_world_;
  effect_->set_ps_MyTex(cube_tex_.get());
  effect_->set_ps_dirlight(0, light_);
  effect_->set_vs_wvp(wvp);
  effect_->set_vs_world(cube_world_);
  effect_->Use(rs);
  rs->Render(cube_vb_.get(), azer::kTriangleList);

  wvp = projection_ * camera_.GenViewMat() * square_world_;
  effect_->set_vs_wvp(wvp);
  effect_->set_vs_world(square_world_);
  effect_->set_ps_MyTex(glass_tex_.get());
  effect_->Use(rs);
  rs->Render(square_vb_.get(), azer::kTriangleList);
}
