#include "azer/render/util/dynlib.h"
#include "azer/base/string.h"
#include "base/logging.h"


#if defined(OS_WIN)
#include <windows.h>
#include <tchar.h>
#    define DYNLIB_HANDLE HINSTANCE
#    define DYNLIB_LOAD(a) LoadLibraryEx(a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
#    define DYNLIB_GETSYM(a, b) GetProcAddress(a, b)
#    define DYNLIB_UNLOAD(a) !FreeLibrary(a)
#    define INVALID_HANDLE  0
#endif

#include "base/logging.h"

namespace azer {
Dynlib::Dynlib(const ::base::FilePath& libpath, bool auto_release)
    : inst_(INVALID_HANDLE)
    , path_(libpath)
    , auto_release_(auto_release) {
}

Dynlib::~Dynlib() {
  if (auto_release_ && inst_ != INVALID_HANDLE) {
    unload();
  }
}
bool Dynlib::load() {
  inst_ = (int64)DYNLIB_LOAD(path_.value().c_str());
  if (NULL == inst_) {
    PLOG(ERROR) << "Failed to Load Dynlib: " << path_.value();
    return false;
  }

  return true;
}

void Dynlib::unload() {
  DYNLIB_UNLOAD((DYNLIB_HANDLE)inst_);
}

void* Dynlib::GetSymbol(const std::string& name) const {
  return DYNLIB_GETSYM((DYNLIB_HANDLE)inst_, name.c_str());
}
}  // namespace azer
