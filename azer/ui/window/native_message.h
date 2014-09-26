#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/ui/window/handle.h"

namespace azer {
class AZER_EXPORT NativeMessage {
 public:
  
  virtual ~Message() {}
 protected:
#if defined(OS_WIN)
  Message(uint32 msg, uint32 wparam, uint32 lparam, WindowHost* host)
      : message_(msg)
      , wparam_(wparam)
      , lparam_(lparam)
      , window_(host) {
  }
#endif

  friend class MessageLoop;
 protected:
#if defined(OS_WIN)
  uint32 message_;
  uint32 wparam_;
  uint32 lparam_;
#endif

  WindowHost* window_;
  DISALLOW_COPY_AND_ASSIGN(Message);
};

class NativeMouseMoveMsg;
class NativeMouseClickMsg;

class AZER_EXPORT NativeMessageDelegate {
 public:
  virtual int OnIdel(NativeMessage* message);

  virtual int OnMouseMove(NativeMouseMoveMsg* msg);
  virtual int OnMouseLeftClick(NativeMouseClickMsg* msg) = 0;
  virtual int OnMouseLeftDClick(NativeMouseClickMsg* msg) = 0;
  virtual int OnMouseRightClick(NativeMouseClickMsg* msg) = 0;
  virtual int OnMouseRightDClick(NativeMouseClickMsg* msg) = 0;
};
}  // namespace azer
