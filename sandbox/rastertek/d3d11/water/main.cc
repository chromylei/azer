#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"

#include "azer/util/util.h"
#include "sandbox/rastertek/d3d11/common/load.h"
#include "ambient.h"
#include "water.h"
#include <tchar.h>

#define EFFECT_GEN_DIR "out/dbg/gen/sandbox/rastertek/d3d11/water/"
#define SHADER_NAME "ambient"
using base::FilePath;

using azer::VertexBuffer;
using azer::RenderSystem;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate() : light_(azer::Light::kDirectionalLight) {}
  virtual void OnCreate() {}

  void Init() {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::ShaderArray shaders;
    CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
    CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));

    effect_.reset(new AmbientEffect(shaders.GetShaderVec(), rs));
    water_effect_.reset(new WaterEffect(shaders.GetShaderVec(), rs));

    // ground
    {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/ground.txt"))));
    ground_data_.reset(new azer::VertexData(effect_->GetVertexDesc(),
                                            vertices.size()));
    memcpy(ground_data_->pointer(), (uint8*)&vertices[0],
           sizeof(AmbientEffect::Vertex) * vertices.size());
    ground_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), ground_data_));
    ground_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/ground01.dds"))));
    }

    {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/wall.txt"))));
    wall_data_.reset(new azer::VertexData(effect_->GetVertexDesc(),
                                            vertices.size()));
    memcpy(wall_data_->pointer(), (uint8*)&vertices[0],
           sizeof(AmbientEffect::Vertex) * vertices.size());
    wall_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), wall_data_));
    wall_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/wall01.dds"))));
    }

    {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/bath.txt"))));
    bath_data_.reset(new azer::VertexData(effect_->GetVertexDesc(),
                                            vertices.size()));
    memcpy(bath_data_->pointer(), (uint8*)&vertices[0],
           sizeof(AmbientEffect::Vertex) * vertices.size());
    bath_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), bath_data_));
    bath_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/marble01.dds"))));
    }

    {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/water.txt"))));
    water_data_.reset(new azer::VertexData(effect_->GetVertexDesc(),
                                            vertices.size()));
    memcpy(water_data_->pointer(), (uint8*)&vertices[0],
           sizeof(AmbientEffect::Vertex) * vertices.size());
    water_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), water_data_));
    water_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/water01.dds"))));
    }
    
    rs->SetViewport(Renderer::Viewport(0, 0, 800, 600));
    rs->SetFrontFace(azer::kCounterClockwise);
    rs->EnableDepthTest(true);
    CHECK(rs->IsDepthTestEnable());
    rs->SetCullingMode(azer::kCullNone);
    
    projection_ = azer::PerspectiveRHD3D(azer::Degree(45.0f), 4.0f / 3.0f,
                                         0.1f, 1000.0f);
    camera_.SetPosition(azer::Vector3(-10.0f, 6.0f, 10.0));
    camera_.SetLookAt(azer::Vector3(.0f, 0.0f, 0.0f));

    grid_.reset(new azer::CoordinateGrid(rs, 20, 20, 20));
    grid_->SetGridDiffuse(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));

    light_.set_directional(azer::Vector4(1.0f, -1.0f, -1.0f, 0.0f));
    light_.set_diffuse(azer::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
    light_.set_ambient(azer::Vector4(0.2f, 0.2f, 0.2f, 1.0f));
  }

  virtual void OnUpdateScene(double time, float delta_time) {
    float rspeed = 3.14f * 2.0f / 2.0f;
    const float speed = 4.0f;
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
    if( ::GetAsyncKeyState('Q') & 0x8000f ) {
      camera_.yaw(azer::Radians(rspeed)* delta_time);
    }
    if( ::GetAsyncKeyState('E') & 0x8000f ) {
      camera_.yaw(azer::Radians(-rspeed) * delta_time);
    }
  }

  virtual void OnRenderScene(double time, float delta_time) {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    DCHECK(NULL != rs);
    
    
    rs->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    rs->ClearDepthAndStencil();
    rs->SetCullingMode(azer::kCullNone);
    
    effect_->set_ps_dirlight(0, light_);

    azer::Matrix4 world = azer::Translate(0.0f, 1.0f, 0.0f);
    azer::Matrix4 wvp = projection_ * camera_.GenViewMat() * world;
    effect_->set_vs_wvp(wvp);
    effect_->set_vs_world(world);
    effect_->set_ps_MyTex(ground_tex_.get());
    effect_->Use(rs);
    rs->Render(ground_vb_.get(), azer::kTriangleList);

    world = azer::Translate(0.0f, 2.0f, 0.0f);
    wvp = projection_ * camera_.GenViewMat() * world;
    effect_->set_vs_wvp(wvp);
    effect_->set_vs_world(world);
    effect_->set_ps_MyTex(bath_tex_.get());
    effect_->Use(rs);
    rs->Render(bath_vb_.get(), azer::kTriangleList);

    world = azer::Translate(0.0f, 6.0f, 8.0f);
    wvp = projection_ * camera_.GenViewMat() * world;
    effect_->set_ps_MyTex(wall_tex_.get());
    effect_->set_vs_wvp(wvp);
    effect_->set_vs_world(world);
    effect_->Use(rs);
    rs->Render(wall_vb_.get(), azer::kTriangleList);

    world = azer::Translate(0.0f, 2.75f, 0.0f);
    wvp = projection_ * camera_.GenViewMat() * world;
    water_effect_->set_ps_dirlight(0, light_);
    water_effect_->set_vs_wvp(wvp);
    water_effect_->set_vs_world(world);
    water_effect_->set_ps_MyTex(water_tex_.get());
    water_effect_->Use(rs);
    rs->Render(water_vb_.get(), azer::kTriangleList);
  }

  virtual void OnQuit() {}
 private:
  azer::VertexDataPtr  ground_data_;
  azer::VertexBufferPtr ground_vb_;
  azer::TexturePtr ground_tex_;

  azer::VertexDataPtr  wall_data_;
  azer::VertexBufferPtr wall_vb_;
  azer::TexturePtr wall_tex_;

  azer::VertexDataPtr  bath_data_;
  azer::VertexBufferPtr bath_vb_;
  azer::TexturePtr bath_tex_;

  azer::VertexDataPtr  water_data_;
  azer::VertexBufferPtr water_vb_;
  azer::TexturePtr water_tex_;
  
  std::unique_ptr<AmbientEffect> effect_;
  std::unique_ptr<WaterEffect> water_effect_;
  std::unique_ptr<azer::CoordinateGrid> grid_;
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
