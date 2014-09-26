#include "azer/ui/window/window.h"

namespace azer {
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

}  // namespace azer
