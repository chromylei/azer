#include "azer/render_system/d3d11/hr_msg.h"

#include <windows.h>
#include <tchar.h>
#include <comdef.h>

#include "azer/base/string.h"
#include "base/strings/utf_string_conversions.h"

namespace azer {
std::string HRMessage(uint32 hr) {
  /*
  string16 msg;
  LPTSTR pvMsgBuf = NULL;
  if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
                   | FORMAT_MESSAGE_FROM_SYSTEM
                   | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL,
                   (DWORD)hr,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                   &pvMsgBuf, 0, NULL)) {
    // Do something with the error message.
    msg.append((LPTSTR)pvMsgBuf);
    LocalFree(pvMsgBuf);
  }
  */
  _com_error error(hr);

  string16 msg = error.ErrorMessage();
  std::string str(::base::UTF16ToUTF8(msg));
  return str;
}
}
