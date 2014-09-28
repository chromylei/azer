#include "azer/ui/window/window_host.h"

#include <windows.h>
#include <tchar.h>

#include "azer/ui/window/native_event.h"
#include "base/logging.h"
#include "base/lazy_instance.h"
#include "base/strings/utf_string_conversions.h"
#include "base/message_loop/message_loop.h"

namespace azer {
namespace {
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_CREATE:
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
  }

  return DefWindowProc(hwnd, msg, wParam, lParam);
}

class AzerWinClassRegister {
 public:
  static AzerWinClassRegister* Pointer() {
    static AzerWinClassRegister* inst = new AzerWinClassRegister;
    return inst;
  }

  static LPCTSTR kMainWndClassName;
  static LPCTSTR kSubWndClassName;
 private:
  void RegisterMainWnd();

  AzerWinClassRegister() {
    RegisterMainWnd();
  }
  ~AzerWinClassRegister() {
  }
};

LPCTSTR AzerWinClassRegister::kMainWndClassName = TEXT("AzerMainWindowHost");
LPCTSTR AzerWinClassRegister::kSubWndClassName = TEXT("AzerSubWindowHost");

void AzerWinClassRegister::RegisterMainWnd() {
  WNDCLASS wc;
  memset(&wc, 0, sizeof(wc));
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = (WNDPROC)WndProc;
  wc.hInstance = ::GetModuleHandle(NULL);
  wc.lpszClassName = kMainWndClassName;

  RECT rc = {0, 0, 800, 600};
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);
  RegisterClass(&wc);
}
}  // namespace

void WindowHost::Init() {
  DCHECK_EQ(handle_, 0);
  AzerWinClassRegister::Pointer();

  if (options_.primary) {
    handle_ = (window::NativeWindowHandle)CreateWindow(
        AzerWinClassRegister::kMainWndClassName,
        ::base::UTF8ToWide(options_.title).c_str(),
        WS_OVERLAPPEDWINDOW,
        options_.left, options_.left,
        options_.width,
        options_.height,
        NULL, NULL,
        ::GetModuleHandle(NULL),
        NULL);
    PCHECK(handle_ != 0);
  } else {
    NOTIMPLEMENTED();
  }

  SetCursor(NULL);
}

void WindowHost::Attach(window::NativeWindowHandle handle) {
  HWND hwnd = (HWND)handle;
  handle_ = handle;
  RECT r;
  GetWindowRect((HWND)handle_, &r);
  options_.left = r.left;
  options_.top = r.top;
  options_.width = r.right - r.left;
  options_.height = r.bottom - r.top;
}

void WindowHost::MainLoop(WindowHost* mainwnd) {
  using namespace azer::window;
  ::base::MessageLoop message_loop(::base::MessageLoop::TYPE_UI);
  MSG msg; 
  memset(&msg, 0, sizeof(msg));
  if (mainwnd->OnInit()) {
    while (msg.message != WM_QUIT) {
      if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      } else {
        NativeIdleEvent e(msg.message, msg.wParam, msg.lParam, mainwnd);
        mainwnd->OnIdle(&e);
      }
    }
  }

  mainwnd->OnQuit();
}
}  // namespace azer
