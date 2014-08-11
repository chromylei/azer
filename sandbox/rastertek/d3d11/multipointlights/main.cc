#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"

#include "azer/util/util.h"
#include "sandbox/rastertek/d3d11/common/load.h"
#include "afx_point_light_effect.h"
#include <tchar.h>

#define EFFECT_GEN_DIR "out/dbg/gen/sandbox/rastertek/d3d11/multipointlights/"
#define SHADER_NAME "afx_point_light_effect"

using azer::VertexBuffer;
using azer::VertexData;
using base::FilePath;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate()
      : light0_(azer::Light::kPointLight)
      , light1_(azer::Light::kPointLight)
      , light2_(azer::Light::kPointLight)
      , light3_(azer::Light::kPointLight)
  {}

  virtual void OnCreate() {}

  void Init();
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);
  virtual void OnQuit() {}
 private:
  std::unique_ptr<PointLightEffect> effect_;

  azer::VertexDataPtr  plane_data_;
  azer::VertexBufferPtr plane_vb_;
  azer::TexturePtr plane_tex_;
  
  azer::Matrix4 projection_;
  azer::Matrix4 plane_world_;
  azer::Camera camera_;
  azer::Light light0_;
  azer::Light light1_;
  azer::Light light2_;
  azer::Light light3_;
  std::unique_ptr<azer::CoordinateGrid> grid_;
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
    
  effect_.reset(new PointLightEffect(shaders.GetShaderVec(), rs));

  {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/plane01.txt"))));
    plane_data_.reset(new VertexData(effect_->GetVertexDesc(),
                                     vertices.size()));
    memcpy(plane_data_->pointer(), (uint8*)&vertices[0],
           sizeof(PointLightEffect::Vertex) * vertices.size());
    plane_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), plane_data_));
    plane_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/stone01.dds"))));
  }
    
  rs->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
  rs->SetFrontFace(azer::kCounterClockwise);
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  rs->SetCullingMode(azer::kCullNone);
  projection_ = azer::PerspectiveRHD3D(azer::Degree(45.0f),
                                       4.0f / 3.0f,
                                       0.1f, 1000.0f);
  camera_.SetLookAt(azer::Vector3(0.0f, 0.0f, 0.0f));
  camera_.SetPosition(0.0, 2.0, 8.0);

  light0_.set_position(azer::Vector4(-3.0f, 1.0f, -3.0f, 1.0f));
  light0_.set_diffuse(azer::Vector4(0.4f, 0.0f, 0.0f, 1.0f));
  light0_.set_ambient(azer::Vector4(0.1f, 0.0f, 0.0f, 1.0f));
  light0_.set_attenuation(azer::Light::Attenuation(0.0f, 0.2f, 0.0f));
  light0_.set_range(2.0f);

  light1_.set_position(azer::Vector4(3.0f, 1.0f, -3.0f, 1.0f));
  light1_.set_diffuse(azer::Vector4(0.0f, 0.4f, 0.0f, 1.0f));
  light1_.set_ambient(azer::Vector4(0.0f, 0.1f, 0.0f, 1.0f));
  light1_.set_attenuation(azer::Light::Attenuation(0.0f, 0.2f, 0.0f));
  light1_.set_range(2.0f);

  light2_.set_position(azer::Vector4(-3.0f, 1.0f, 3.0f, 1.0f));
  light2_.set_diffuse(azer::Vector4(0.0f, 0.0f, 0.4f, 1.0f));
  light2_.set_ambient(azer::Vector4(0.0f, 0.0f, 0.1f, 1.0f));
  light2_.set_attenuation(azer::Light::Attenuation(0.0f, 0.2f, 0.0f));
  light2_.set_range(2.0f);

  light3_.set_position(azer::Vector4(3.0f, 1.0f, 3.0f, 1.0f));
  light3_.set_diffuse(azer::Vector4(0.4f, 0.0f, 0.4f, 1.0f));
  light3_.set_ambient(azer::Vector4(0.1f, 0.0f, 0.1f, 1.0f));
  light3_.set_attenuation(azer::Light::Attenuation(0.0f, 0.2f, 0.0f));
  light3_.set_range(2.0f);


  grid_.reset(new azer::CoordinateGrid(rs, 20, 20, 20));
  grid_->SetGridDiffuse(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
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

  plane_world_ = azer::mat4::kIdentity;
  azer::mat4 wvp = projection_ * camera_.GenViewMat() * plane_world_;
  effect_->set_ps_ambient(azer::vec3(0.2f, 0.2f, 0.2f));
  effect_->set_ps_MyTex(plane_tex_.get());
  effect_->set_vs_wvp(wvp);
  effect_->set_vs_world(plane_world_);
  effect_->set_ps_pointlights(0, light0_);
  effect_->set_ps_pointlights(1, light1_);
  effect_->set_ps_pointlights(2, light2_);
  effect_->set_ps_pointlights(3, light3_);
  effect_->set_ps_light_num(4);
  effect_->Use(rs);
  rs->Render(plane_vb_.get(), azer::kTriangleList);

  /*
  grid_->SetViewProjMat(projection_ * camera_.GenViewMat());
  grid_->SetGridDiffuse(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
  grid_->Render(rs);
  */
}
