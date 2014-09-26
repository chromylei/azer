#include "azer/util/sampleapp.h"
#include "base/logging.h"

#include "base/strings/utf_string_conversions.h"
#include "azer/render/util/render_system_loader.h"
#include "azer/render/render_system.h"

SampleApp::SampleApp(const Options& opt, Delegate* delegate)
    : azer::WindowHost(opt)
    , delegate_(delegate)
    , render_system_(NULL) {
  DCHECK(delegate_ != NULL);
}

SampleApp::SampleApp(Delegate* delegate)
    : azer::WindowHost(Options())
    , delegate_(delegate)
    , render_system_(NULL) {
  DCHECK(delegate_ != NULL);
}

bool SampleApp::OnInit() {
  CHECK(azer::LoadRenderSystem(this));
  DCHECK(render_system_ == NULL);
  render_system_ = azer::RenderSystem::Current();

  DCHECK(delegate_ != NULL);
  if (!delegate_->OnInit()) {
    return false;
  }

  prev_frame_ = ::base::TimeTicks::HighResNow();
  delegate_->OnUpdateScene(0, 0);
  return true;
}

void SampleApp::OnQuit() {
  DCHECK(delegate_ != NULL);
  delegate_->OnQuit();
}

int SampleApp::OnIdle(azer::window::NativeIdleMsg* message) {
  DCHECK(delegate_ != NULL);
  ::base::TimeTicks now = ::base::TimeTicks::HighResNow();
  ::base::TimeDelta delta = now - prev_frame_;
  total_ticking_ += delta;
  prev_frame_ = now;
  double total = total_ticking_.InSecondsF();
  delegate_->OnRenderScene(total, (float)delta.InSecondsF());
  delegate_->OnUpdateScene(total, (float)delta.InSecondsF());
  render_system_->Present();
  return kSuccess;
}

void SampleApp::MainLoop() {
  Show();
  WindowHost::MainLoop(this);
}
