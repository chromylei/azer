#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"

#include "azer/util/util.h"
#include "sandbox/rastertek/d3d11/common/load.h"
#include "afx_shadow_map_effect.h"
#include "afx_depth_effect.h"
#include "afx_surface_blur_effect.h"
#include <tchar.h>

const float weights[] = {0.1f, 0.55f, 0.9f, 1.0f, 0.9f, 0.55f, 0.1f};
int weight_num = arraysize(weights);

#define EFFECT_GEN_DIR "out/dbg/gen/sandbox/rastertek/d3d11/soft_shadow/"
#define SHADER_NAME "afx_shadow_map_effect"
#define DEPTH_SHADER_NAME "afx_depth_effect"
#define BLUR_SHADER_NAME "afx_surface_blur_effect"

using azer::VertexBuffer;
using azer::VertexData;
using base::FilePath;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate()
      : light0_(azer::Light::kPointLight)
      , camera_(azer::vec4(0, 1, 4, 1),
                azer::vec4(0, 0, 0, 1),
                azer::vec4(0, 1, 0, 0))
                {}
  virtual void OnCreate() {}

  void Init();
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);
  virtual void OnQuit() {}
 private:
  std::shared_ptr<ShadowMapEffect> effect_;
  std::shared_ptr<DepthEffect> depth_effect_;
  std::shared_ptr<OverlayBlurEffect> blur_effect_;
  
  azer::VertexDataPtr  plane_data_;
  azer::VertexBufferPtr plane_vb_;
  azer::TexturePtr plane_tex_;
  azer::Matrix4 plane_world_;

  azer::VertexDataPtr  cube_data_;
  azer::VertexBufferPtr cube_vb_;
  azer::TexturePtr cube_tex_;
  azer::Matrix4 cube_world_;

  azer::VertexDataPtr  sphere_data_;
  azer::VertexBufferPtr sphere_vb_;
  azer::TexturePtr sphere_tex_;
  azer::Matrix4 sphere_world_;
  
  azer::Matrix4 projection_;
  azer::Matrix4 view_projection_;
  azer::Camera camera_;
  azer::Light light0_;
  azer::OverlayPtr full_surface_;

  std::unique_ptr<azer::Renderer> sm_renderer0_;
  std::unique_ptr<azer::Renderer> sm_renderer1_;
  bool show_cube_;
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
  azer::Renderer* renderer = rs->GetDefaultRenderer();
  azer::ShaderArray shaders;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
  effect_.reset(new ShadowMapEffect(shaders.GetShaderVec(), rs));

  azer::ShaderArray shaders2;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR DEPTH_SHADER_NAME ".vs", &shaders2));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR DEPTH_SHADER_NAME ".ps", &shaders2));
  depth_effect_.reset(new DepthEffect(shaders2.GetShaderVec(), rs));

  azer::ShaderArray shaders3;
  CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR BLUR_SHADER_NAME ".vs", &shaders3));
  CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR BLUR_SHADER_NAME ".ps", &shaders3));
  blur_effect_.reset(new OverlayBlurEffect(shaders3.GetShaderVec(), rs));

  renderer->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
  renderer->SetFrontFace(azer::kCounterClockwise);
  renderer->EnableDepthTest(true);
  CHECK(renderer->IsDepthTestEnable());
  renderer->SetCullingMode(azer::kCullNone);
  projection_ = azer::PerspectiveRHD3D(azer::Degree(45.0f),
                                       4.0f / 3.0f,
                                       1.0f, 1000.0f);

  view_projection_ = azer::PerspectiveRHD3D(azer::Degree(90.0f),
                                            4.0f / 3.0f,
                                            1.0f, 1000.0f);

  light0_.set_position(azer::Vector4(0.0f, 12.0f, 4.0f, 1.0f));
  light0_.set_diffuse(azer::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
  light0_.set_ambient(azer::Vector4(0.1f, 0.1f, 0.1f, 1.0f));
  light0_.set_attenuation(azer::Light::Attenuation(0.0f, 0.1f, 0.0f));
  light0_.set_range(1000.0f);

  azer::Texture::SamplerState sampler;
  sampler.mag_filter = azer::Texture::kLinear;
  sampler.min_filter = azer::Texture::kLinear;
  sampler.mipmag_filter = azer::Texture::kLinear;
  sampler.mipmin_filter = azer::Texture::kLinear;
  {
  azer::Texture::Options opt;
  opt.width = 800;
  opt.height = 600;
  opt.sampler = sampler;
  opt.format = azer::kRGBAf;
  opt.sampler.compare_func = azer::CompareFunc::kLessEqual;
  opt.sampler.wrap_u = azer::Texture::kClamp;
  opt.sampler.wrap_v = azer::Texture::kClamp;
  opt.sampler.wrap_w = azer::Texture::kClamp;
  opt.target = (azer::Texture::BindTarget)
      (azer::Texture::kRenderTarget | azer::Texture::kShaderResource);
  sm_renderer0_.reset(rs->CreateRenderer(opt));
  }

  {
  azer::Texture::Options opt;
  opt.width = 800;
  opt.height = 600;
  opt.sampler = sampler;
  opt.format = azer::kRGBAf;
  opt.sampler.wrap_u = azer::Texture::kClamp;
  opt.sampler.wrap_v = azer::Texture::kClamp;
  opt.sampler.wrap_w = azer::Texture::kClamp;
  opt.sampler.compare_func = azer::CompareFunc::kLessEqual;
  opt.target = (azer::Texture::BindTarget)
      (azer::Texture::kRenderTarget | azer::Texture::kShaderResource);
  sm_renderer1_.reset(rs->CreateRenderer(opt));
  }

  full_surface_.reset(rs->CreateOverlay(gfx::RectF(-1, -1, 2, 2)));
  full_surface_->SetEffect(blur_effect_);
  blur_effect_->SetTexture(sm_renderer0_->GetRenderTarget()->GetTexture());
  blur_effect_->set_vs_screen_width(800.0f);
  blur_effect_->set_vs_screen_height(600.0f);
  // blur_effect_->set_ps_weights(weights, arraysize(weights));

  {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/plane01.txt"))));
    plane_data_.reset(new VertexData(effect_->GetVertexDesc(),
                                     vertices.size()));
    memcpy(plane_data_->pointer(), (uint8*)&vertices[0],
           sizeof(ShadowMapEffect::Vertex) * vertices.size());
    plane_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), plane_data_));
    plane_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/metal001.dds"))));
  }

  {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/cube.txt"))));
    cube_data_.reset(new VertexData(effect_->GetVertexDesc(),
                                    vertices.size()));
    memcpy(cube_data_->pointer(), (uint8*)&vertices[0],
           sizeof(ShadowMapEffect::Vertex) * vertices.size());
    cube_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), cube_data_));
    cube_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/wall01.dds"))));
  }

  {
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/sphere.txt"))));
    sphere_data_.reset(new VertexData(effect_->GetVertexDesc(),
                                    vertices.size()));
    memcpy(sphere_data_->pointer(), (uint8*)&vertices[0],
           sizeof(ShadowMapEffect::Vertex) * vertices.size());
    sphere_vb_.reset(rs->CreateVertexBuffer(VertexBuffer::Options(), sphere_data_));
    sphere_tex_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/ice.dds"))));
  }
}

void MainDelegate::OnUpdateScene(double time, float delta_time) {
  float rspeed = 3.14f * 2.0f / 4.0f / 4.0f;
  const float speed = 2.0f;
  cube_world_ = azer::Translate(-3.0, 1.0, 0.0);

  sphere_world_ = azer::Translate(3.0, 1.0, 0.0);

  azer::vec4 pos = azer::vec4(0, 12, 12, 1) * azer::RotateY(
      azer::Radians(rspeed * time));
  light0_.set_position(pos);
  
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

  if( ::GetAsyncKeyState('Z') & 0x8000f ) {
    show_cube_ = !show_cube_;
  }
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);

  plane_world_ = azer::mat4::kIdentity;
  azer::mat4 proj_view = azer::LookAtRH(light0_.position(),
                                       azer::vec4(0, 0, 0, 1),
                                       azer::vec4(0, 1, 0, 0));
  azer::mat4 projtex_vp = view_projection_ * proj_view;
  azer::mat4 projtex_wvp;
  azer::mat4 wvp;
  {
    sm_renderer0_->Use();
    sm_renderer0_->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
    sm_renderer0_->Clear(azer::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    sm_renderer0_->ClearDepthAndStencil();
    sm_renderer0_->SetCullingMode(azer::kCullNone);
    sm_renderer0_->EnableDepthTest(true);
    sm_renderer0_->SetCullingMode(azer::kCullNone);

    wvp = projtex_vp * sphere_world_;
    depth_effect_->set_vs_wvp(wvp);
    depth_effect_->Use(sm_renderer0_.get());
    sm_renderer0_->Render(sphere_vb_.get(), azer::kTriangleList);

    wvp = projtex_vp * cube_world_;
    depth_effect_->set_vs_wvp(wvp);
    depth_effect_->Use(sm_renderer0_.get());
    sm_renderer0_->Render(cube_vb_.get(), azer::kTriangleList);
    
    wvp = projtex_vp * plane_world_;
    depth_effect_->set_vs_wvp(wvp);
    depth_effect_->Use(sm_renderer0_.get());
    sm_renderer0_->Render(plane_vb_.get(), azer::kTriangleList);
  }

  {
    sm_renderer1_->Use();
    sm_renderer1_->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
    sm_renderer1_->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    sm_renderer1_->ClearDepthAndStencil();
    sm_renderer1_->SetCullingMode(azer::kCullNone);
    sm_renderer1_->EnableDepthTest(true);
    full_surface_->Render(sm_renderer1_.get());
  }

  azer::Renderer* renderer = rs->GetDefaultRenderer();
  renderer->Use();
  renderer->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  renderer->ClearDepthAndStencil();
  renderer->SetCullingMode(azer::kCullNone);
  wvp = projection_ * camera_.GenViewMat() * plane_world_;
  azer::TexturePtr depth_texture = sm_renderer1_->GetRenderTarget()->GetTexture();
  projtex_wvp =  projtex_vp * plane_world_;
  effect_->set_ps_MyTex(plane_tex_);
  effect_->set_ps_ProjTex(depth_texture);
  effect_->set_ps_ptlight(0, light0_);
  effect_->set_vs_wvp(wvp);
  effect_->set_vs_world(plane_world_);
  effect_->set_vs_projtex_wvp(projtex_wvp);
  effect_->Use(renderer);
  renderer->Render(plane_vb_.get(), azer::kTriangleList);

  if (show_cube_) {
  wvp = projection_ * camera_.GenViewMat() * cube_world_;
  projtex_wvp =  projtex_vp * cube_world_;
  effect_->set_ps_MyTex(cube_tex_);
  effect_->set_ps_ProjTex(depth_texture);
  effect_->set_ps_ptlight(0, light0_);
  effect_->set_vs_wvp(wvp);
  effect_->set_vs_world(cube_world_);
  effect_->set_vs_projtex_wvp(projtex_wvp);
  effect_->Use(renderer);
  renderer->Render(cube_vb_.get(), azer::kTriangleList);
  }

  wvp = projection_ * camera_.GenViewMat() * sphere_world_;
  projtex_wvp =  projtex_vp * sphere_world_;
  effect_->set_ps_MyTex(sphere_tex_);
  effect_->set_ps_ProjTex(depth_texture);
  effect_->set_ps_ptlight(0, light0_);
  effect_->set_vs_wvp(wvp);
  effect_->set_vs_world(sphere_world_);
  effect_->set_vs_projtex_wvp(projtex_wvp);
  effect_->Use(renderer);
  renderer->Render(sphere_vb_.get(), azer::kTriangleList);
  
}
