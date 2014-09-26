#include "azer/ui/window/native_message_handler.h"
#include "azer/ui/window/native_message.h"

namespace azer {
namespace window {
int NativeMessageHandler::OnIdle(NativeIdleMsg* message) {
  return kDefAction;
}

int NativeMessageHandler::OnMouseMove(NativeMouseMoveMsg* msg) {
  return kDefAction;
}

int NativeMessageHandler::OnMouseLeftClick(NativeMouseClickMsg* msg) {
  return kDefAction;
}

int NativeMessageHandler::OnMouseLeftDClick(NativeMouseClickMsg* msg) {
  return kDefAction;
}

int NativeMessageHandler::OnMouseRightClick(NativeMouseClickMsg* msg) {
  return kDefAction;
}

int NativeMessageHandler::OnMouseRightDClick(NativeMouseClickMsg* msg) {
  return kDefAction;
}

int NativeMessageHandler::OnKeyUp(NativeKeyMsg* msg) {
  return kDefAction;
}

int NativeMessageHandler::OnKeyDown(NativeKeyMsg* msg) {
  return kDefAction;
}

int NativeMessageHandler::OnChar(NativeKeyMsg* msg) {
  return kDefAction;
}
}  // namespace window
}  // namespace azer
