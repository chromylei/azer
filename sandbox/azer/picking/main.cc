#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"
#include "sandbox/azer/common/loadobj.h"
#include "sandbox/azer/common/camera.h"
#include "sandbox/azer/common/skysphere.h"
#include "sandbox/azer/common/aabb.h"
#include "sandbox/azer/common/raymesh.h"
#include <tchar.h>
#include <xnamath.h>

const int kWindowWidth = 800;
const int kWindowHeight = 600;

azer::WindowHost *gMainwin = NULL;

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate();
  void Init();
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time);
  virtual void OnRenderScene(double time, float delta_time);

  virtual void OnQuit() {}
 private:
  void InitLight();
  void InitBottles();
  void RenderBottle(azer::RenderSystem* rs);
  void RenderGround(azer::RenderSystem* rs);
  void CheckClick();
  azer::Ray GetCameraRay();

  std::unique_ptr<azer::Technique> tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  std::string vertex_shader_;
  std::string fragment_shader_;
  
  azer::GpuConstantsTablePtr vs_table_;
  azer::GpuConstantsTablePtr ps_table_;
  azer::Mesh mesh_;
  azer::Mesh bottle_;
  azer::Light light_;
  Camera camera_;
  SkySphere sphere_;

  std::unique_ptr<azer::Technique> bottle_tech_;
  azer::GpuProgramPtr bottle_vs_;
  azer::GpuProgramPtr bottle_fs_;
  std::string bottle_vertex_shader_;
  std::string bottle_fragment_shader_;

  azer::AxisAlignedBoundingBox aabb_;
  std::unique_ptr<AABBMesh> aabb_mesh_;
  RayMesh raymesh_;

  struct BottleInfo {
    azer::vec3 pos;
    bool click;
  };

  azer::Ray ray_;
  float last_click_;
  XMMATRIX WVP;
  XMMATRIX meshworld;
  std::vector<BottleInfo> bottle_info_;
  static const azer::VertexDesc::Desc kVertexDescs[];
};

::base::FilePath vertex_shader_path(TEXT("sandbox/azer/picking/ground.vs.hlsl"));
::base::FilePath pixel_shader_path(TEXT("sandbox/azer/picking/ground.ps.hlsl"));

::base::FilePath bottle_vertex_shader_path(TEXT("sandbox/azer/picking/bottle.vs.hlsl"));
::base::FilePath bottle_pixel_shader_path(TEXT("sandbox/azer/picking/bottle.ps.hlsl"));

MainDelegate::MainDelegate()
    : light_(azer::Light::kDirectionalLight)
    , last_click_(0.0f) {
}

void MainDelegate::Init() {
  CHECK(::base::ReadFileToString(vertex_shader_path, &vertex_shader_));
  CHECK(::base::ReadFileToString(pixel_shader_path, &fragment_shader_));

  CHECK(::base::ReadFileToString(bottle_vertex_shader_path, &bottle_vertex_shader_));
  CHECK(::base::ReadFileToString(bottle_pixel_shader_path, &bottle_fragment_shader_));
  
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  rs->SetViewport(azer::RenderSystem::Viewport(0, 0, kWindowWidth, kWindowHeight));
  azer::GpuConstantsTable::Desc dvs_desc[] = {
    azer::GpuConstantsTable::Desc("WVP", azer::GpuConstantsType::kMatrix4, 1),
    azer::GpuConstantsTable::Desc("WORLD", azer::GpuConstantsType::kMatrix4, 1),
  };
  vs_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(dvs_desc), dvs_desc));

  azer::GpuConstantsTable::Desc pvs_desc[] = {
    azer::GpuConstantsTable::Desc("light",
                                  azer::GpuConstantsType::kDirectionalLight, 1),
  };
  ps_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(pvs_desc), pvs_desc));
  
  ObjMeshLoader objloader1, objloader2;
  CHECK(objloader1.Load(base::FilePath(TEXT("sandbox/azer/media/ground.obj")),
                        &mesh_)) << "Failed to load obj";
  CHECK(objloader2.Load(base::FilePath(TEXT("sandbox/azer/media/bottle.obj")),
                        &bottle_)) << "Failed to load obj";
  
  aabb_ = azer::AxisAlignedBoundingBox::CalcForMesh(bottle_);

  vs_.reset(rs->CreateVertexGpuProgram(mesh_.Subs()->at(0)->vb_ptr->desc(),
                                       vertex_shader_));
  fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, fragment_shader_));
  tech_.reset(rs->CreateEffect());
  tech_->AddGpuProgram(vs_);
  tech_->AddGpuProgram(fs_);
  tech_->Finalize();
  
  bottle_vs_.reset(rs->CreateVertexGpuProgram(mesh_.Subs()->at(0)->vb_ptr->desc(),
                                              bottle_vertex_shader_));
  bottle_fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, bottle_fragment_shader_));
  bottle_tech_.reset(rs->CreateEffect());
  bottle_tech_->AddGpuProgram(bottle_vs_);
  bottle_tech_->AddGpuProgram(bottle_fs_);
  bottle_tech_->Finalize();

  InitLight();
  sphere_.Init();
  InitBottles();

  aabb_mesh_.reset(new AABBMesh(rs));
  raymesh_.Init();
  // objloader_.dump();
}

azer::Ray MainDelegate::GetCameraRay() {
  POINT pt;
  RECT rc;
  ::GetCursorPos(&pt);
  ::ScreenToClient((HWND)gMainwin->Handle(), &pt);
  ::GetClientRect((HWND)gMainwin->Handle(), &rc);
  LOG(ERROR) << "click pt(" << pt.x << ", " << pt.y << ")"
             << " rect(" << rc.left << ", " << rc.top << ", "
             << rc.right - rc.left << ", " << rc.bottom - rc.top << ")";
      
  float view_x =  (float)2 * pt.x / (float)(rc.right - rc.left) - 1;
  float view_y = -(float)2 * pt.y / (float)(rc.bottom - rc.top) + 1;
  float view_z = 1.0f;
  view_x /= camera_.projection()(0, 0);
  view_y /= camera_.projection()(1, 1);
  view_z = 1.0f;
  LOG(ERROR) << "click pos: " << view_x << ", " << view_y << ", " << view_z;
  
  XMVECTOR xna_clickpt = XMVectorSet(view_x, view_y, view_z, 1.0);
  XMVECTOR v = XMMatrixDeterminant(camera_.view());
  XMMATRIX invView = XMMatrixInverse(&v, camera_.view());
  xna_clickpt = XMVector3TransformCoord(xna_clickpt, invView);
  LOG(ERROR) << "click pos: " << XMVectorGetX(xna_clickpt) << ", "
             << XMVectorGetY(xna_clickpt) << ", "
             << XMVectorGetZ(xna_clickpt);
  XMVECTOR xna_dir = xna_clickpt - camera_.position();
  azer::Vector3 pos(XMVectorGetX(camera_.position()),
                    XMVectorGetY(camera_.position()),
                    XMVectorGetZ(camera_.position()));
  azer::Vector3 dir(XMVectorGetX(xna_dir),
                    XMVectorGetY(xna_dir),
                    XMVectorGetZ(xna_dir));
  dir.Normalize();
  LOG(ERROR) << "Check click Ray : " << pos << " dir:" << dir;
  return azer::Ray(pos, dir);
}

void MainDelegate::CheckClick() {
  azer::Ray ray = GetCameraRay();
  ray_ = ray;

  for (auto iter = bottle_info_.begin(); iter != bottle_info_.end(); ++iter) {
    if (!iter->click) {
      azer::Matrix4 mat = azer::Transpose(iter->pos);
      // azer::Matrix4 mat = azer::Transpose(azer::vec3(0.0f, 0.0f, 0.0f));
      azer::AxisAlignedBoundingBox aabb = aabb_.TransformCopy(mat);
      if (aabb.intersect(ray)) {
        LOG(ERROR) << "clicked";
        iter->click = true;
      }
      // break;
    }
  }
}


void MainDelegate::OnUpdateScene(double time, float delta_time) {
  camera_.OnInput();
  XMMATRIX Translation = XMMatrixTranslation( 0.0f, 0.0f, 0.0f );
  meshworld = Translation;


  if (time - last_click_ > 0.5) {
    if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000f) {
      last_click_ = time;
      CheckClick();
    }
  }
}

void MainDelegate::OnRenderScene(double time, float delta_time) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  DCHECK(NULL != rs);
  rs->Clear(azer::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  rs->EnableDepthTest(true);
  CHECK(rs->IsDepthTestEnable());
  rs->ClearDepthAndStencil();

  raymesh_.Render(&camera_, ray_.position(),
                  ray_.position() + ray_.directional() * 100);

  // sphere_.Render(camera_);

  RenderGround(rs);
  RenderBottle(rs);
  rs->Present();
}

void MainDelegate::InitLight() {
  light_.set_directional(azer::vec4(0.25f, 0.5f, -1.0f, 0.0f));
  light_.set_ambient(azer::vec4(0.2f, 0.2f, 0.2f, 1.0f));
  light_.set_diffuse(azer::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

void MainDelegate::RenderGround(azer::RenderSystem* rs) {
  WVP = meshworld * camera_.view() * camera_.projection();
  XMMATRIX twvp = XMMatrixTranspose(WVP);
  XMMATRIX tworld = XMMatrixTranspose(meshworld);
  tech_->Use();
  vs_table_->SetValue(0, (void*)&twvp, sizeof(twvp));
  vs_table_->SetValue(1, (void*)&tworld, sizeof(tworld));
  vs_table_->flush();
  rs->UseConstantsTable(azer::kVertexStage,vs_table_.get());
  ps_table_->SetLight(0, light_);
  rs->UseConstantsTable(azer::kPixelStage, ps_table_.get());
  ps_table_->flush();

  rs->SetFillMode(azer::kSolid);
  rs->SetCullingMode(azer::kCullNone);
  mesh_.Render(rs);
}

void MainDelegate::RenderBottle(azer::RenderSystem* rs) {
  for (auto iter = bottle_info_.begin(); iter != bottle_info_.end(); ++iter) {
    if (!iter->click) {
      XMMATRIX world = XMMatrixTranslation(iter->pos.x, iter->pos.y, iter->pos.z);
      // XMMATRIX world = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
      WVP = world * camera_.view() * camera_.projection();
      XMMATRIX twvp = XMMatrixTranspose(WVP);
      XMMATRIX tworld = XMMatrixTranspose(world);
      bottle_tech_->Use();
      vs_table_->SetValue(0, (void*)&twvp, sizeof(twvp));
      vs_table_->SetValue(1, (void*)&tworld, sizeof(tworld));
      vs_table_->flush();
      ps_table_->SetLight(0, light_);
      ps_table_->flush();
      rs->UseConstantsTable(azer::kVertexStage,vs_table_.get());
      rs->UseConstantsTable(azer::kPixelStage, ps_table_.get());
      rs->SetFillMode(azer::kSolid);
      rs->SetCullingMode(azer::kCullNone);
      bottle_.Render(rs);

      azer::Matrix4 mat = azer::Transpose(iter->pos);
      azer::AxisAlignedBoundingBox aabb = aabb_.TransformCopy(mat);
      aabb_mesh_->Render(rs, aabb, camera_);
    }
  }
}

void MainDelegate::InitBottles() {
  float bottleXPos = -30.0f;
  float bottleZPos = 30.0f;
  float bxadd = 0.0f;
  float bzadd = 0.0f;
  for (int i = 0; i < 100; ++i) {
    BottleInfo info;
    bxadd += 1.0f;
    if (bxadd == 10) {
      bzadd -= 1.0f;
      bxadd = 0.0f;
    }

    info.click = false;
    info.pos = azer::vec3(bottleXPos + bxadd * 10.0f,
                          2.0f,
                          bottleZPos + bzadd * 10.0f);
    bottle_info_.push_back(info);
  }
}

const TCHAR* dll_path = TEXT("out/dbg/d3d11_render_system.dll");

int main(int argc, char* argv[]) {
  ::base::InitApp(&argc, &argv, "");
  

  azer::WindowHost::Options opt;
  opt.width = kWindowWidth;
  opt.height = kWindowHeight;
  MainDelegate delegate;
  azer::WindowHost win(opt, &delegate);
  gMainwin = &win;
  win.Init();
  CHECK(azer::RenderSystem::InitRenderSystem(::base::FilePath(dll_path), &win));
  LOG(ERROR) << "Current RenderSystem: " << azer::RenderSystem::Current()->name();
  delegate.Init();
  
  // initialize shader
  
  win.Show();

  azer::MainRenderLoop(&win);
  return 0;
}

