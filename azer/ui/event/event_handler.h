#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"

namespace azer {
namespace ui {

class Event;

class AZER_EXPORT EventHandler {
 public:
  EventHandler();
  virtual ~EventHandler();

  // This is called for all events. The default implementation routes the event
  // to one of the event-specific callbacks (OnKeyEvent, OnMouseEvent etc.). If
  // this is overridden, then normally, the override should chain into the
  // default implementation for un-handled events.
  virtual void OnEvent(Event* event);
  virtual void OnKeyEvent(KeyEvent* event);
  virtual void OnMouseEvent(MouseEvent* event);
  virtual void OnScrollEvent(ScrollEvent* event);
  virtual void OnTouchEvent(TouchEvent* event);
  virtual void OnGestureEvent(GestureEvent* event);
  virtual void OnCancelMode(CancelModeEvent* event);
 protected:
  DISALLOW_COPY_AND_ASSIGN(EventHandler);
};

}  // namespace ui
}  // namespace azer
