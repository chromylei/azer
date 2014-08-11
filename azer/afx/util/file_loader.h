#pragma once

#include <map>
#include <string>
#include <vector>

#include "azer/base/string.h"
#include "base/basictypes.h"
#include "base/files/file_path.h"

namespace azer {
namespace afx {
class FileLoader {
 public:
  FileLoader(const std::vector<std::string>& includedir)
      : include_dirs_(includedir) {
  }

  virtual bool Load(const std::string& file, std::string* content,
                    ::base::FilePath* real);
 protected:
  virtual bool LoadFileToString(const ::base::FilePath& path, std::string* content);
 private:
  const std::vector<std::string>& include_dirs_;

  DISALLOW_COPY_AND_ASSIGN(FileLoader);
};

class MemoryFileLoader : public FileLoader {
 public:
  MemoryFileLoader(const std::vector<std::string>& inc) : FileLoader(inc) {}

  void AddFile(const azer::StringType& path, const std::string& content) {
    ::base::FilePath fullpath(path);
    content_.insert(std::make_pair(fullpath.value(), content));
  }
 protected:
  virtual bool LoadFileToString(const ::base::FilePath& path,
                                std::string* content) OVERRIDE;
  std::map<azer::StringType, std::string> content_;
  DISALLOW_COPY_AND_ASSIGN(MemoryFileLoader);
};
}  // namespace afx
}  // namespace azer
