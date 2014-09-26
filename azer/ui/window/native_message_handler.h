#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/ui/window/native_handle.h"

namespace azer {
namespace window {

class NativeMessage;
class NativeIdleMsg;
class NativeMouseMoveMsg;
class NativeMouseClickMsg;
class NativeKeyMsg;

class AZER_EXPORT NativeMessageHandler {
 public:
  NativeMessageHandler() {}
  virtual ~NativeMessageHandler() {}

  enum {
    kDefAction = -1,
    kSuccess = 0,
  };

  virtual int OnIdle(NativeIdleMsg* message);

  virtual int OnMouseMove(NativeMouseMoveMsg* msg);
  virtual int OnMouseLeftClick(NativeMouseClickMsg* msg);
  virtual int OnMouseLeftDClick(NativeMouseClickMsg* msg);
  virtual int OnMouseRightClick(NativeMouseClickMsg* msg);
  virtual int OnMouseRightDClick(NativeMouseClickMsg* msg);

  virtual int OnKeyUp(NativeKeyMsg* msg);
  virtual int OnKeyDown(NativeKeyMsg* msg);
  virtual int OnChar(NativeKeyMsg* msg);
 protected:
  DISALLOW_COPY_AND_ASSIGN(NativeMessageHandler);
};

}  // namespace window
}  // namespace azer
