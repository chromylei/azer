#include "azer/afx/util/file_loader.h"

#include "azer/base/string.h"
#include "base/files/file_path.h"
#include "base/file_util.h"
#include "base/strings/utf_string_conversions.h"

namespace azer {
namespace afx {
bool FileLoader::Load(const std::string& file, std::string* content,
                      ::base::FilePath* finalpath) {
  for (auto iter = include_dirs_.begin(); iter != include_dirs_.end();
       ++iter) {
    ::base::FilePath fullpath(::base::UTF8ToWide(*iter));
    fullpath = fullpath.AppendASCII(file);
    if (LoadFileToString(fullpath, content)) {
      *finalpath = fullpath.NormalizePathSeparators();
      return true;
    } else {
      return false;
    }
  }

  ::base::FilePath fullpath(::base::UTF8ToWide(file));
  if (LoadFileToString(fullpath, content)) {
    *finalpath = fullpath;
    return true;
  } else {
    return false;
  }
}

bool FileLoader::LoadFileToString(const ::base::FilePath& path,
                                  std::string* content) {
  if (! ::base::PathExists(path)) {
    return false;
  }

  return ::base::ReadFileToString(path, content);
}

bool MemoryFileLoader::LoadFileToString(const ::base::FilePath& path,
                                        std::string* content) {
  auto iter = content_.find(path.value());
  if (iter != content_.end()) {
    *content = iter->second;
    return true;
  } else {
    return false;
  }
}

}  // namespace afx
}  // namespace azer
