#include "azer/render/render.h"
#include "azer/math/math.h"
#include "base/base.h"
#include "base/file_util.h"
#include "sandbox/azer/common/camera2.h"
#include "sandbox/azer/common/skysphere.h"
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
  std::unique_ptr<azer::Technique> tech_;
  azer::GpuProgramPtr vs_;
  azer::GpuProgramPtr fs_;
  azer::VertexDataPtr  data_;
  azer::VertexBufferPtr vb_;
  azer::IndicesBufferPtr ib_;
  azer::GpuConstantsTablePtr vs_table_;
  azer::GpuConstantsTablePtr ps_table_;
  azer::TexturePtr tex_;
  azer::Light light_;

  std::string vertex_shader_;
  std::string fragment_shader_;

  azer::Matrix4 groundWorld;
  Camera2 camera_;
  SkySphere sphere_;
  static const azer::VertexDesc::Desc kVertexDescs[];
 public:
  static const int kWindowWidth = 800;
  static const int kWindowHeight = 600;
};


