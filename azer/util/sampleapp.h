#pragma once

#include "azer/ui/window/window_host.h"
#include "base/time/time.h"

namespace azer {
class RenderSystem;
}  // namespace azer

class SampleApp : public azer::WindowHost {
 public:
  class Delegate {
   public:
    virtual bool OnInit() = 0;
    virtual void OnUpdateScene(double time, float delta_time) = 0;
    virtual void OnRenderScene(double time, float delta_time) = 0;
    virtual void OnQuit() = 0;
  };

  SampleApp(const Options& opt, Delegate* delegate);
  SampleApp(Delegate* delegate);
  azer::RenderSystem* GetRS() { return render_system_;}

  void MainLoop();
 protected:
  virtual bool OnInit();
  virtual void OnQuit();
  virtual int OnIdle(azer::window::NativeIdleEvent* message);
  Delegate* delegate_;
  ::base::TimeDelta total_ticking_;
  base::TimeTicks prev_frame_;
  azer::RenderSystem* render_system_;
  DISALLOW_COPY_AND_ASSIGN(SampleApp);
};
