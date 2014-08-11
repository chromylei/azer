#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"

#include "azer/util/util.h"
#include "sandbox/rastertek/d3d11/common/load.h"
#include "DiffuseEffect.h"
#include <tchar.h>

#define EFFECT_GEN_DIR "out/dbg/gen/sandbox/rastertek/d3d11/culling/"
#define SHADER_NAME "DiffuseEffect"
using base::FilePath;

const azer::Vector3 kPosition[] = {
  azer::Vector3(0.0f, 0.0f, -8.0f),
  azer::Vector3(1.0f, 0.0f, -21.0f),
  azer::Vector3(-3.0f, 7.0f, -21.0f),
  azer::Vector3(-3.0f, 2.0f, -21.0f),
  azer::Vector3(0.0f, 2.0f, -11.0f),
  azer::Vector3(0.0f, 0.0f, -7.0f),
  azer::Vector3(-3.0f, 0.0f, -5.0f),
  azer::Vector3(-8.0f, 11.0f, -5.0f),
  azer::Vector3(-32.0f, -32.0f, -5.0f),
  azer::Vector3(-54.0f, -8.0f, -18.0f),
  azer::Vector3(23.0f, -5.0f, -38.0f),
  azer::Vector3(13.0f, 17.0f, -51.0f),
  azer::Vector3(33.0f, 21.0f, 0.0f),
  azer::Vector3(19.0f, 9.0f, -5.0f),
  azer::Vector3(11.0f, -8.0f, -7.0f),
  azer::Vector3(17.0f, -3.0f, -11.0f),
};

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  virtual void OnCreate() {}

  void Init() {
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::ShaderArray shaders;
    CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
    CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
    
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/sphere.txt"))));
    effect_.reset(new DiffuseEffect(shaders.GetShaderVec(), rs));
    data_.reset(new azer::VertexData(effect_->GetVertexDesc(), vertices.size()));
    memcpy(data_->pointer(), (uint8*)&vertices[0],
           sizeof(DiffuseEffect::Vertex) * vertices.size());
    vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));
    rs->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
    rs->SetFrontFace(azer::kCounterClockwise);
    rs->EnableDepthTest(true);
    CHECK(rs->IsDepthTestEnable());
    rs->SetCullingMode(azer::kCullNone);

    tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/seafloor.dds"))));
    projection_ = azer::PerspectiveRHD3D(azer::Degree(45.0f),
                                         4.0f / 3.0f,
                                         0.1f, 1000.0f);
    camera_.SetPosition(azer::Vector3(0.0f, 3.0f, 5.0f));

    grid_.reset(new azer::CoordinateGrid(rs, 20, 20, 20));
    grid_->SetGridDiffuse(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
  }

  virtual void OnUpdateScene(double time, float delta_time) {
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

    for (int i = 0; i < arraysize(kPosition); ++i) {
      azer::Matrix4 world = std::move(azer::Translate(kPosition[i]));
      azer::Matrix4 wvp = projection_ * camera_.GenViewMat() * world;
      rs->SetCullingMode(azer::kCullNone);
      effect_->set_ps_MyTex(tex_.get());
      effect_->set_vs_wvp(wvp);
      effect_->set_vs_world(world);
      effect_->Use(rs);
      rs->Render(vb_.get(), azer::kTriangleList);
    }

    grid_->SetViewProjMat(projection_ * camera_.GenViewMat());
    grid_->SetGridDiffuse(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
    grid_->Render(rs);
  }

  virtual void OnQuit() {}
 private:
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  std::unique_ptr<DiffuseEffect> effect_;
  std::unique_ptr<azer::CoordinateGrid> grid_;
  azer::Matrix4 projection_;
  azer::Camera camera_;
  azer::TexturePtr tex_;
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
