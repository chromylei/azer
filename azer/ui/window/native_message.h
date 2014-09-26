#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/ui/window/native_handle.h"

namespace azer {
namespace window {
class Window;
class AZER_EXPORT NativeMessage {
 public:
  virtual ~NativeMessage() {}
  enum MessageType {
    kUnknownMsg = -1,
    kIdleMsg = 1,
  };

  MessageType type() const { return type_; }
 protected:
#if defined(OS_WIN)
  NativeMessage(uint32 msg, uint32 wparam, uint32 lparam, Window* host,
                MessageType type)
      : message_(msg)
      , wparam_(wparam)
      , lparam_(lparam)
      , window_(host)
      , type_(type) {
  }
#endif

  friend class MessageLoop;
 protected:
#if defined(OS_WIN)
  uint32 message_;
  uint32 wparam_;
  uint32 lparam_;
#endif
  
  Window* window_;
  MessageType type_;
  DISALLOW_COPY_AND_ASSIGN(NativeMessage);
};

class NativeIdleMsg : public NativeMessage {
 public:
  NativeIdleMsg(uint32 msg, uint32 wparam, uint32 lparam, Window* host)
      : NativeMessage(msg, wparam, lparam, host, kIdleMsg) {
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(NativeIdleMsg);
};

}  // namespace window
}  // namespace azer

