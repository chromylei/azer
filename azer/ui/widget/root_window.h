#pragma once

#include "base/basictypes.h"
#include "azer/ui/widget/widget.h"

namespace gfx {
class Canvas;
}

namespace azer {

class WindowHost;
class RenderSystem;

class AZER_WIDGET_EXPORT RootWindow : public Widget {
 public:
  virtual ~RootWindow() {}
  static RootWindow* Create(RenderSystem* rs);

  virtual void Render();
  void Redraw() { this->Redraw(NULL);}
  

  RenderSystem* GetRenderSystem() { return render_system_;}
  WindowHost* host() { return host_;}
 protected:
  
  RootWindow(RenderSystem* rs)
      : Widget(gfx::Rect())
      , render_system_(rs) {
  }

  virtual void Redraw(gfx::Canvas* canvas);

  RenderSystem* render_system_;
  WindowHost* host_;
  bool Init();
  DISALLOW_COPY_AND_ASSIGN(RootWindow);
};
}  // namespace azer
