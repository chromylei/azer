#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/ui/window/native_handle.h"
#include "azer/ui/window/native_event.h"
#include "azer/ui/window/native_event_handler.h"
#include "ui/gfx/rect.h"

namespace azer {
namespace window {
class AZER_EXPORT Window : public NativeEventHandler {
 public:
  Window() : handle_(kInvalidWindowHandle) {}

  NativeWindowHandle Handle() const { return handle_;}

  gfx::Rect GetClientBounds() const;
  void SetBounds(const gfx::Rect& rect);

  void Show();
  void Hide();
 protected:
  NativeWindowHandle handle_;
  DISALLOW_COPY_AND_ASSIGN(Window);
};
}  // namespace window
}  // namespace azer
