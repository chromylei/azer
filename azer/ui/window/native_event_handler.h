#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/ui/window/native_handle.h"

namespace azer {
namespace window {

class NativeEvent;
class NativeIdleEvent;
class NativeMouseMoveEvent;
class NativeMouseClickEvent;
class NativeKeyEvent;

class AZER_EXPORT NativeEventHandler {
 public:
  NativeEventHandler() {}
  virtual ~NativeEventHandler() {}

  enum {
    kDefAction = -1,
    kSuccess = 0,
  };

  virtual int OnIdle(NativeIdleEvent* message);

  virtual int OnMouseMove(NativeMouseMoveEvent* msg);
  virtual int OnMouseLeftClick(NativeMouseClickEvent* msg);
  virtual int OnMouseLeftDClick(NativeMouseClickEvent* msg);
  virtual int OnMouseRightClick(NativeMouseClickEvent* msg);
  virtual int OnMouseRightDClick(NativeMouseClickEvent* msg);

  virtual int OnKeyUp(NativeKeyEvent* msg);
  virtual int OnKeyDown(NativeKeyEvent* msg);
  virtual int OnChar(NativeKeyEvent* msg);
 protected:
  DISALLOW_COPY_AND_ASSIGN(NativeEventHandler);
};

}  // namespace window
}  // namespace azer
