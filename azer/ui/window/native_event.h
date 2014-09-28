#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/ui/window/native_handle.h"

namespace azer {
namespace window {
class Window;
class AZER_EXPORT NativeEvent {
 public:
  virtual ~NativeEvent() {}
  enum EventType {
    kUnknownEvent = -1,
    kIdleEvent = 1,
  };

  EventType type() const { return type_; }
 protected:
#if defined(OS_WIN)
  NativeEvent(uint32 msg, uint32 wparam, uint32 lparam, Window* host,
                EventType type)
      : message_(msg)
      , wparam_(wparam)
      , lparam_(lparam)
      , window_(host)
      , type_(type) {
  }
#endif

  friend class EventLoop;
 protected:
#if defined(OS_WIN)
  uint32 message_;
  uint32 wparam_;
  uint32 lparam_;
#endif
  
  Window* window_;
  EventType type_;
  DISALLOW_COPY_AND_ASSIGN(NativeEvent);
};

class NativeIdleEvent : public NativeEvent {
 public:
  NativeIdleEvent(uint32 msg, uint32 wparam, uint32 lparam, Window* host)
      : NativeEvent(msg, wparam, lparam, host, kIdleEvent) {
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(NativeIdleEvent);
};

}  // namespace window
}  // namespace azer

