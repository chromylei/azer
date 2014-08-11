#include "azer/render/test/util.h"

#include "azer/util/util.h"
#include "azer/render/render.h"

namespace azer {
namespace testing {

class TestWinDelegate : public azer::WindowHost::Delegate {
 public:
  TestWinDelegate() {}
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time) {}
  virtual void OnRenderScene(double time, float delta_time) {}
  virtual void OnQuit() {}
 private:
  DISALLOW_COPY_AND_ASSIGN(TestWinDelegate);
};

RenderSystem* CreateD3D11RenderSystemForTest() {
  TestWinDelegate delegate;
  azer::WindowHost win(azer::WindowHost::Options(), &delegate);
  win.Init();
  CHECK(azer::LoadRenderSystem(&win));
  return RenderSystem::Current();
}
}  // namespace testing
}  // namespace azer
