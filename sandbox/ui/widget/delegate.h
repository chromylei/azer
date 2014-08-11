#include "azer/render/render.h"
#include "azer/ui/widget/widget_all.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"
#include "sandbox/azer/common/camera2.h"
#include "sandbox/azer/common/skysphere.h"
#include "sandbox/azer/common/grid.h"
#include "azer/render/util/frame_counter.h"
#include <tchar.h>
#include <xnamath.h>

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
  void InitConstantTable();
  void InitVertexAndIndices();
  azer::Light light_;

  azer::Matrix4 gridworld_;
  Camera2 camera_;
  azer::Matrix4 mapProjection;

  SkySphere sphere_;
  Grid grid_;
  
  std::unique_ptr<azer::RootWindow> root_ui_;
  azer::Label* fps_label_;
  azer::FrameCounter counter_;
  static const azer::VertexDesc::Desc kVertexDescs[];
 public:
  static const int kWindowWidth = 800;
  static const int kWindowHeight = 600;
};


