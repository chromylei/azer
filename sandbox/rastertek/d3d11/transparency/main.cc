#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "azer/util/util.h"
#include "sandbox/rastertek/d3d11/common/load.h"
#include "texeffect.h"
#include <tchar.h>

#define EFFECT_GEN_DIR "out/dbg/gen/sandbox/rastertek/d3d11/transparency/"
#define SHADER_NAME "texeffect"
using base::FilePath;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  virtual void OnCreate() {}

  void Init() {
    CommandLine* cmd = CommandLine::ForCurrentProcess();
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    azer::ShaderArray shaders;
    CHECK(azer::LoadVertexShader(EFFECT_GEN_DIR SHADER_NAME ".vs", &shaders));
    CHECK(azer::LoadPixelShader(EFFECT_GEN_DIR SHADER_NAME ".ps", &shaders));
    
    std::vector<Vertex> vertices = std::move(
        loadModel(base::FilePath(TEXT("sandbox/rastertek/media/square.txt"))));
    effect_.reset(new TextureEffect(shaders.GetShaderVec(), rs));
    data_.reset(new azer::VertexData(effect_->GetVertexDesc(), vertices.size()));
    memcpy(data_->pointer(), (uint8*)&vertices[0],
           sizeof(TextureEffect::Vertex) * vertices.size());
    vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data_));
    rs->SetViewport(azer::Renderer::Viewport(0, 0, 800, 600));
    rs->SetFrontFace(azer::kCounterClockwise);
    rs->EnableDepthTest(true);
    CHECK(rs->IsDepthTestEnable());
    rs->SetCullingMode(azer::kCullNone);

    tex1_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/stone01.dds"))));
    tex2_.reset(rs->CreateTextureFromFile(
        azer::Texture::k2D,
        base::FilePath(TEXT("sandbox/rastertek/media/dirt01.dds"))));
    projection_ = azer::PerspectiveRHD3D(azer::Degree(45.0f),
                                         4.0f / 3.0f,
                                         0.1f, 1000.0f);
    camera_.SetPosition(azer::Vector3(0.0f, 0.0f, 0.0f));

    grid_.reset(new azer::CoordinateGrid(rs, 20, 20, 20));
    grid_->SetGridDiffuse(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));

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

  virtual void OnUpdateScene(double time, float delta_time) {
    float rspeed = 3.14f * 2.0f / 4.0f;
    /*
      world_ = azer::Translate(0.0, 0.0, -8.0)
        * azer::RotateY(azer::Radians(time * rspeed));
        */

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

  virtual void OnRenderScene(double time, float delta_time) {
    float blend_factor[] = {0.75f, 0.75f, 0.75f, 1.0f};
    azer::RenderSystem* rs = azer::RenderSystem::Current();
    DCHECK(NULL != rs);

    rs->ResetBlending();
    blending_->Use(blend_factor);
    world_ = azer::Translate(0.0, 0.0, -6.0);
    azer::Matrix4 wvp = projection_ * camera_.GenViewMat() * world_;
    rs->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    rs->ClearDepthAndStencil();
    rs->SetCullingMode(azer::kCullNone);
    effect_->set_ps_Tex(tex1_.get());
    effect_->set_vs_wvp(wvp);
    effect_->set_vs_world(world_);
    effect_->Use(rs);
    rs->Render(vb_.get(), azer::kTriangleList);

    world_ = azer::Translate(1.0, 0.0, -4.0);
    wvp = projection_ * camera_.GenViewMat() * world_;
    effect_->set_ps_Tex(tex2_.get());
    effect_->set_vs_wvp(wvp);
    effect_->set_vs_world(world_);
    effect_->Use(rs);
    rs->Render(vb_.get(), azer::kTriangleList);
    grid_->SetViewProjMat(projection_ * camera_.GenViewMat());
    grid_->SetGridDiffuse(azer::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
    grid_->Render(rs);
  }

  virtual void OnQuit() {}
 private:
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  std::unique_ptr<TextureEffect> effect_;
  std::unique_ptr<azer::CoordinateGrid> grid_;
  azer::Matrix4 projection_;
  azer::Matrix4 world_;
  azer::Camera camera_;
  azer::TexturePtr tex1_;
  azer::TexturePtr tex2_;
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
