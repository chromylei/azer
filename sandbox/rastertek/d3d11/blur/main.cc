#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"


#include "azer/util/util.h"
#include "sandbox/rastertek/d3d11/common/load.h"
#include "ambient.h"
#include <tchar.h>

#define EFFECT_GEN_DIR "out/dbg/gen/sandbox/rastertek/d3d11/blur/"
#define SHADER_NAME "ambient"
using base::FilePath;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate() : light_(azer::Light::kDirectionalLight) {}
  virtual void OnCreate() {}  
  void Init();
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);
  virtual void OnQuit() {}
 private:
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  std::unique_ptr<AmbientEffect> effect_;
  std::unique_ptr<azer::CoordinateGrid> grid_;
  azer::Matrix4 projection_;
  azer::Matrix4 world_;
  azer::Camera camera_;
  azer::TexturePtr tex_;
  azer::Light light_;
  std::unique_ptr<azer::RenderTarget> render_target_;
  std::unique_ptr<azer::DepthBuffer> depth_buffer_;

  azer::SurfaceEffectPtr surface_effect_;
  std::unique_ptr<azer::Surface> full_surface_;
  std::unique_ptr<azer::Surface> half_surface_;
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
    
  std::vector<Vertex> vertices = std::move(
      loadModel(base::FilePath(TEXT("sandbox/rastertek/media/cube.txt"))));
  effect_.reset(new AmbientEffect(shaders.GetShaderVec(), rs));
  data_.reset(new azer::VertexData(effect_->GetVertexDesc(), vertices.size()));
  memcpy(data_->pointer(), (uint8*)&vertices[0],
         sizeof(AmbientEffect::Vertex) * vertices.size());
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));
  rs->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
  rs->SetFrontFace(azer::kCounterClockwise);
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  rs->SetCullingMode(azer::kCullNone);

  tex_.reset(rs->CreateTextureFromFile(
      azer::Texture::k2D,
      base::FilePath(TEXT("sandbox/rastertek/media/stone01.dds"))));
  projection_ = azer::PerspectiveRHD3D(azer::Degree(45.0f),
                                       4.0f / 3.0f,
                                       0.1f, 1000.0f);
  camera_.SetPosition(azer::Vector3(0.0f, 0.0f, 0.0f));

  grid_.reset(new azer::CoordinateGrid(rs, 20, 20, 20));
  grid_->SetGridDiffuse(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));

  light_.set_directional(azer::Vector4(1.0f, 0.0f, 0.0f, 0.0f));
  light_.set_diffuse(azer::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
  light_.set_ambient(azer::Vector4(0.4f, 0.4f, 0.4f, 1.0f));

  azer::Texture::Options opt;
  opt.width = 800;
  opt.height = 600;
  opt.target = (azer::Texture::BindTarget)
      (azer::Texture::kRenderTarget | azer::Texture::kShaderResource);
  render_target_.reset(rs->CreateRenderTarget(opt));

  opt.target = azer::Texture::kDepthStencil;
  opt.format = azer::kDepth24Stencil8;
  depth_buffer_.reset(rs->CreateDepthBuffer(opt));

  full_surface_.reset(rs->CreateSurface(gfx::RectF(-1, -1, 2, 2)));
  half_surface_.reset(rs->CreateSurface(gfx::RectF(-0.5, -0.5, 1, 1)));
  surface_effect_.reset(full_surface_->CreateDefaultEffect());
  full_surface_->SetEffect(surface_effect_);
  half_surface_->SetEffect(surface_effect_);
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  float rspeed = 3.14f * 2.0f / 4.0f;
  world_ = azer::Translate(0.0, 0.0, -8.0)
      * azer::RotateY(azer::Radians(time * rspeed));

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
  rs->EnableDepthTest(true);
  {
    azer::Matrix4 wvp = projection_ * camera_.GenViewMat() * world_;
    azer::ScopedRenderTarget scoped_render_target(rs);
    rs->SetRenderTarget(1, render_target_.get(), depth_buffer_.get());
    rs->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
    rs->EnableDepthTest(true);
    depth_buffer_->Clear();
    render_target_->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 0.0f));

    effect_->set_ps_MyTex(tex_.get());
    effect_->set_vs_wvp(wvp);
    effect_->set_vs_world(world_);
    effect_->set_ps_dirlight(0, light_);
    effect_->Use(rs);
    rs->Render(vb_.get(), azer::kTriangleList);
  }

  rs->EnableDepthTest(false);
  surface_effect_->SetTexture(render_target_->GetTexture());
  full_surface_->Render(rs);
  // half_surface_->Draw(render_target_->GetTexture(), rs);
}
