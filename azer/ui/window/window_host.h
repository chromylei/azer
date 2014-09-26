/**
 * ui 框架采用 DirectDraw 设计
 * 在此框架下大部分 widget (窗口，控件) 没有 HWND(window 平台下),
 * 每一个 widget 仅仅是某个能够绘制矩形的窗口而已
 * WindowHost 是所有这些矩形的归属，一个真正的 HWND
 */
#pragma once

#include "base/basictypes.h"
#include "base/time/time.h"
#include "azer/ui/window/window.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/rect.h"

namespace azer {
class RenderSystem;

class AZER_EXPORT WindowHost : public window::Window {
 public:
  struct Options {
    int32 left;
    int32 top;
    int32 width;
    int32 height;
    bool fullscreen;
    bool primary;
    char title[256];
    Options()
        : left(100)
        , top(100)
        , width(800)
        , height(600)
        , fullscreen(false)
        , primary(true) {
      memset(title, 0, sizeof(title));
    }
  };

  class Delegate {
   public:
    virtual void OnCreate() = 0;
    virtual void OnUpdateScene(double time, float delta_time) = 0;
    virtual void OnRenderScene(double time, float delta_time) = 0;
    virtual void OnQuit() = 0;
  };

  explicit WindowHost(const Options& opt);
  WindowHost(const Options& opt, Delegate* delegate);
  ~WindowHost();

  virtual bool OnInit() {return true;};
  virtual void OnQuit() {};

  void Init();
  void SetDelegate(Delegate* delegate) {delegate_ = delegate;}
  void Attach(window::NativeWindowHandle handle);
  const Options& GetMetrics() const { return options_;}

  static void MainLoop(WindowHost* mainwnd);
  friend void AZER_EXPORT MainRenderLoop(WindowHost* mainwnd);

  void SetRenderSystem(RenderSystem* rs) { render_system_ = rs;}
  RenderSystem* GetRenderSystem() { return render_system_;}
 private:
  Delegate* delegate_;
  RenderSystem* render_system_;
  Options options_;
  DISALLOW_COPY_AND_ASSIGN(WindowHost);
};
}  // namespace azer
