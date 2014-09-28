#include "azer/ui/window/native_event_handler.h"
#include "azer/ui/window/native_event.h"

namespace azer {
namespace window {
int NativeEventHandler::OnIdle(NativeIdleEvent* event) {
  return kDefAction;
}

int NativeEventHandler::OnMouseMove(NativeMouseMoveEvent* msg) {
  return kDefAction;
}

int NativeEventHandler::OnMouseLeftClick(NativeMouseClickEvent* msg) {
  return kDefAction;
}

int NativeEventHandler::OnMouseLeftDClick(NativeMouseClickEvent* msg) {
  return kDefAction;
}

int NativeEventHandler::OnMouseRightClick(NativeMouseClickEvent* msg) {
  return kDefAction;
}

int NativeEventHandler::OnMouseRightDClick(NativeMouseClickEvent* msg) {
  return kDefAction;
}

int NativeEventHandler::OnKeyUp(NativeKeyEvent* msg) {
  return kDefAction;
}

int NativeEventHandler::OnKeyDown(NativeKeyEvent* msg) {
  return kDefAction;
}

int NativeEventHandler::OnChar(NativeKeyEvent* msg) {
  return kDefAction;
}
}  // namespace window
}  // namespace azer
