#include "azer/ui/widget/root_window.h"

#include "azer/render/render_system.h"
#include "azer/render/win/window_host.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/font.h"
#include "ui/gfx/font_list.h"

namespace azer {

RootWindow* RootWindow::Create(RenderSystem* rs) {
  std::unique_ptr<RootWindow> rwin(new RootWindow(rs));
  if (rwin->Init()) {
    return rwin.release();
  } else {
    return NULL;
  }
}

bool RootWindow::Init() {
  host_ = render_system_->GetRenderWindowHost();
  rect_ = std::move(host_->GetClientBounds());
  rect_.Offset(-rect_.x(), -rect_.y());

  gfx::FontList::SetDefaultFontDescription("Droid Sans serif, Sans serif, 14px");
  return true;
}

void RootWindow::Render() {
  /*
   * render in reverse order
   */
  Widget* widget = last_child();
  while (widget) {
    // DCHECK(widget->HasSurface());
    widget->Render();
    widget = widget->prev_sibling();
  }
}

void RootWindow::Redraw(gfx::Canvas* canvas) {
  /*
   * render in reverse order
   */
  Widget* widget = last_child();
  while (widget) {
    // DCHECK(widget->HasSurface());
    widget->Redraw(NULL);
    widget = widget->prev_sibling();
  }
}
}  // namespace azer
