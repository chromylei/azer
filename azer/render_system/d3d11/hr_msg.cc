#include "azer/render_system/d3d11/hr_msg.h"

#include <windows.h>
#include <tchar.h>

#include "azer/base/string.h"
#include "base/strings/utf_string_conversions.h"

namespace base {
string16 HRMessage(uint32 hr) {
  string16 msg;
  LPVOID pvMsgBuf = NULL;
  if(FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER
                    | FORMAT_MESSAGE_FROM_SYSTEM
                    | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    (DWORD)hr,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (LPTSTR)&pvMsgBuf, 0, NULL)) {
    // Do something with the error message.
    msg.append((LPTSTR)pvMsgBuf);
    LocalFree(pvMsgBuf);
  }

  return msg;
}
}
