#include "azer/ui/window/window_host.h"

#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>
#include <tchar.h>

#include "base/command_line.h"
#include "base/time/time.h"
#include "base/files/file_path.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/message_loop/message_loop.h"
#include "azer/render/render_system.h"


namespace azer {

namespace {
int GenerateDump(EXCEPTION_POINTERS* pExceptionPointers);
}  // namespace

void MainRenderLoop(WindowHost* mainwnd) {
  ::base::MessageLoop message_loop(::base::MessageLoop::TYPE_UI);
  ::base::TimeDelta total_ticking;
  base::TimeTicks prev_frame = ::base::TimeTicks::HighResNow();
  MSG msg; 
  memset(&msg, 0, sizeof(msg));
  if (mainwnd->delegate_) {
    mainwnd->delegate_->OnUpdateScene(0, 0);
  }

  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      ::base::TimeTicks now = ::base::TimeTicks::HighResNow();
      ::base::TimeDelta delta = now - prev_frame;
      total_ticking += delta;
      prev_frame = now;
      double total = total_ticking.InSecondsF();
      if (mainwnd->delegate_) {
        mainwnd->delegate_->OnRenderScene(total, (float)delta.InSecondsF());
        mainwnd->delegate_->OnUpdateScene(total, (float)delta.InSecondsF());
      }

      DCHECK(mainwnd->render_system_ != NULL);
      mainwnd->render_system_->Present();
    }
  }

  if (mainwnd->delegate_) {
    mainwnd->delegate_->OnQuit();
  }
}

namespace {
int GenerateDump(EXCEPTION_POINTERS* pExceptionPointers) {
  BOOL bMiniDumpSuccessful;
  HANDLE hDumpFile;
  SYSTEMTIME stLocalTime;
  MINIDUMP_EXCEPTION_INFORMATION ExpParam;

  CommandLine* cmdline = CommandLine::ForCurrentProcess();
  std::string pathstr;
  GetLocalTime(&stLocalTime);
  base::SStringPrintf(&pathstr,
                      "%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp",
                      ::base::UTF16ToUTF8(cmdline->GetProgram().value()).c_str(),
                      stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
                      stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
                      GetCurrentProcessId(), GetCurrentThreadId());

  base::FilePath filepath(::base::UTF8ToWide(pathstr));
  hDumpFile = CreateFile(filepath.value().c_str(),
                         GENERIC_READ|GENERIC_WRITE, 
                         FILE_SHARE_WRITE|FILE_SHARE_READ,
                         0,
                         CREATE_ALWAYS, 0, 0);
  
  ExpParam.ThreadId = GetCurrentThreadId();
  ExpParam.ExceptionPointers = pExceptionPointers;
  ExpParam.ClientPointers = TRUE;
  
  bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(),
                                          GetCurrentProcessId(), 
                                          hDumpFile, MiniDumpWithDataSegs,
                                          &ExpParam, NULL, NULL);
  return EXCEPTION_EXECUTE_HANDLER;
}

}
}  // namespace azer
