#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"

#include "azer/util/util.h"
#include "sandbox/rastertek/d3d11/common/load.h"
#include "ambient.h"
#include "mirror.h"
#include <tchar.h>

#define EFFECT_GEN_DIR "out/dbg/gen/sandbox/rastertek/d3d11/reflection/"
#define SHADER_NAME "ambient"
#define MSHADER_NAME "mirror"

using azer::VertexBuffer;
using azer::VertexData;
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
  azer::VertexDataPtr  cube_data_;
  azer::VertexBufferPtr cube_vb_;
  std::unique_ptr<AmbientEffect> cube_effect_;
  std::unique_ptr<MirrorEffect> mirror_effect_;
  azer::TexturePtr cube_tex_;

  azer::VertexDataPtr  plane_data_;
  azer::VertexBufferPtr plane_vb_;
  azer::TexturePtr plane_tex_;
  
  azer::Matrix4 projection_;
  azer::Matrix4 cube_world_;
  azer::Matrix4 plane_world_;
  azer::Camera camera_;
  azer::Light light_;
  std::unique_ptr<azer::RenderTarget> render_target_;
  std::unique_ptr<azer::DepthBuffer> depth_buffer_;
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
  cube_effect_.reset(new AmbientEffect(shaders.GetShaderVec(), rs));

  azer::ShaderArray shaders2;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR MSHADER_NAME ".vs", &shaders2));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR MSHADER_NAME ".ps", &shaders2));
  mirror_effect_.reset(new MirrorEffect(shaders2.GetShaderVec(), rs));

  {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/cube.txt"))));
    cube_data_.reset(new VertexData(cube_effect_->GetVertexDesc(),
                                    vertices.size()));
    memcpy(cube_data_->pointer(), (uint8*)&vertices[0],
           sizeof(AmbientEffect::Vertex) * vertices.size());
    cube_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), cube_data_));
    cube_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/seafloor.dds"))));
  }

  {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/floor.txt"))));
    plane_data_.reset(new VertexData(cube_effect_->GetVertexDesc(),
                                     vertices.size()));
    memcpy(plane_data_->pointer(), (uint8*)&vertices[0],
           sizeof(AmbientEffect::Vertex) * vertices.size());
    plane_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), plane_data_));
    plane_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/blue01.dds"))));
  }
    
  rs->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
  rs->SetFrontFace(azer::kCounterClockwise);
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  rs->SetCullingMode(azer::kCullNone);
  projection_ = azer::PerspectiveRHD3D(azer::Degree(45.0f),
                                       4.0f / 3.0f,
                                       0.1f, 1000.0f);
  camera_.SetPosition(azer::Vector3(0.0f, 0.0f, 8.0f));
  camera_.SetLookAt(azer::Vector3(0.0f, 0.0f, 0.0f));
  camera_.SetPosition(0.0, 0.0, 8.0);

  light_.set_directional(azer::Vector4(1.0f, -1.0f, 0.0f, 0.0f));
  light_.set_diffuse(azer::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
  light_.set_ambient(azer::Vector4(0.2f, 0.2f, 0.2f, 1.0f));

  azer::Texture::Options opt;
  opt.width = 800;
  opt.height = 600;
  opt.target = (azer::Texture::BindTarget)
      (azer::Texture::kRenderTarget | azer::Texture::kShaderResource);
  render_target_.reset(rs->CreateRenderTarget(opt));

  opt.target = azer::Texture::kDepthStencil;
  opt.format = azer::kDepth24Stencil8;
  depth_buffer_.reset(rs->CreateDepthBuffer(opt));
  grid_.reset(new azer::CoordinateGrid(rs, 20, 20, 20));
  grid_->SetGridDiffuse(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  float rspeed = 3.14f * 2.0f / 4.0f;
  cube_world_ = azer::Translate(0.0, 0.0, -3.0)
      * azer::RotateY(azer::Radians(time * rspeed));

  plane_world_ = azer::Translate(0.0, -1.5, 0.0);

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
    
  azer::Matrix4 wvp;
  azer::Vector3 pos = camera_.position();
  azer::Vector3 cpos2 = azer::Vector3(pos.x, -1.5 - (pos.y - (-1.5f)), pos.z);
  azer::Vector3 lookat = cpos2 + camera_.direction();
  azer::Vector3 up = -camera_.up();
  azer::mat4 reflect_view = azer::LookAtRH(cpos2, lookat, up);
  azer::mat4 reflect_vp = projection_ * reflect_view;
  azer::mat4 reflect_cube_wvp = reflect_vp * cube_world_;
  {
    azer::ScopedRenderTarget scoped_render_target(rs);
    rs->SetRenderTarget(1, render_target_.get(), depth_buffer_.get());
    depth_buffer_->Clear();
    render_target_->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    rs->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
    rs->SetCullingMode(azer::kCullNone);
    rs->EnableDepthTest(true);
    cube_effect_->set_ps_dirlight(0, light_);
    cube_effect_->set_ps_MyTex(cube_tex_.get());
    cube_effect_->set_vs_wvp(reflect_cube_wvp);
    cube_effect_->set_vs_world(cube_world_);
    cube_effect_->Use(rs);
    rs->Render(cube_vb_.get(), azer::kTriangleList);
  }

  rs->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  rs->ClearDepthAndStencil();
  rs->SetCullingMode(azer::kCullNone);

  wvp = projection_ * camera_.GenViewMat() * cube_world_;
  cube_effect_->set_ps_dirlight(0, light_);
  cube_effect_->set_ps_MyTex(cube_tex_.get());
  cube_effect_->set_vs_wvp(wvp);
  cube_effect_->set_vs_world(cube_world_);
  cube_effect_->Use(rs);
  rs->Render(cube_vb_.get(), azer::kTriangleList);

  wvp = projection_ * camera_.GenViewMat() * plane_world_;
  azer::mat4 reflect_plane_wvp = reflect_vp * plane_world_;
  mirror_effect_->set_ps_MirrorTex(plane_tex_.get());
  mirror_effect_->set_ps_ReflectTex(render_target_->GetTexture().get());
  mirror_effect_->set_vs_wvp(wvp);
  mirror_effect_->set_vs_reflect_wvp(reflect_plane_wvp);
  mirror_effect_->Use(rs);
  rs->Render(plane_vb_.get(), azer::kTriangleList);

  grid_->SetViewProjMat(projection_ * camera_.GenViewMat());
  grid_->SetGridDiffuse(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
  grid_->Render(rs);
}
