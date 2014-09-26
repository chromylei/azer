#include "azer/ui/window/window.h"

#include <windows.h>
#include "base/logging.h"

namespace azer {
namespace window {
void Window::Show() {
  DCHECK_GT(handle_, 0);
  ShowWindow((HWND)handle_, SW_SHOW);
  UpdateWindow((HWND)handle_);
}

gfx::Rect Window::GetClientBounds() const {
  RECT r;
  GetClientRect((HWND)handle_, &r);
  return gfx::Rect(r);
}
}  // namespace window
}  // namespace azer
