#pragma once

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include <string>

namespace azer {
class Dynlib {
 public:
  /**
   * auto_release: 在对象结束时是否自动释放加载的 DLL
   * 默认为 true
   * Note: 对于某些加载以后一直运行的程序，一定要将 auto_release 设置为 false
   */
  Dynlib(const ::base::FilePath& libpath, bool auto_release = true);
  ~Dynlib();
  bool load();
  void unload();

  void* GetSymbol(const std::string& name) const;
 private:
  ::base::FilePath path_;
  int64 inst_;
  bool auto_release_;
};
}  // namespace azer
