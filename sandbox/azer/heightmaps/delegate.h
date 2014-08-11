#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"
#include "sandbox/azer/common/camera2.h"
#include "sandbox/azer/common/skysphere.h"
#include "sandbox/azer/common/grid.h"
#include <tchar.h>

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
  SkySphere sphere_;
  Grid grid_;
  static const azer::VertexDesc::Desc kVertexDescs[];
 public:
  static const int kWindowWidth = 800;
  static const int kWindowHeight = 600;
};


