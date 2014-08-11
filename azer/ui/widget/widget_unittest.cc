#include "gtest/gtest.h"


#include "azer/ui/widget/widget_all.h"
#include "azer/render/render.h"

/**
 * 此 unittest 仅仅测试与界面与渲染无关的函数
 */

class MainDelegate : public azer::WindowHost::Delegate {
 public:
  MainDelegate() {}
  void Init();
  virtual void OnCreate() {}
  virtual void OnUpdateScene(double time, float delta_time) {}
  virtual void OnRenderScene(double time, float delta_time) {}

  virtual void OnQuit() {}
};

class WidgetTest : public testing::Test {
 public:
  virtual void SetUp() {
    const TCHAR* dll_path = TEXT("out/dbg/d3d11_render_system.dll");
    azer::WindowHost::Options opt;
    opt.width = kWinWidth;
    opt.height = kWinHeight;
    win_.reset(new azer::WindowHost(opt, &delegate_));
    win_->Init();
    CHECK(azer::RenderSystem::InitRenderSystem(
        ::base::FilePath(dll_path), win_.get()));
  }
  virtual void TearDown() {
    azer::RenderSystem::ReleaseRenderSystem();
  }
 private:
  static const int kWinWidth = 800;
  static const int kWinHeight = 600;
  MainDelegate delegate_;
  std::unique_ptr<azer::WindowHost> win_;
};

TEST_F(WidgetTest, RootWindow) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  std::unique_ptr<azer::RootWindow> root(azer::RootWindow::Create(rs));

  azer::Window* win = azer::Window::Create(gfx::Rect(100, 100, 400, 300),
                                           root.get());
}

TEST_F(WidgetTest, Tree) {
  
}

TEST_F(WidgetTest, ConvertPointToHost) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  std::unique_ptr<azer::RootWindow> root(azer::RootWindow::Create(rs));

  azer::Window* win = azer::Window::Create(gfx::Rect(100, 100, 400, 300),
                                           root.get());
  azer::StaticText* stext = azer::StaticText::Create(
      gfx::Rect(10, 20, 100, 20), win);
  gfx::Point pt = stext->ConvertPointToSurface(gfx::Point(5, 5));
  ASSERT_EQ(pt, gfx::Point(15, 25));

  
}

TEST(Widget, ConvertRectToSurface) {
}
