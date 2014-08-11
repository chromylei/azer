#pragma once

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "azer/base/render_export.h"
#include "azer/base/string.h"
#include "azer/resources/content.h"
#include "base/lazy_instance.h"

namespace azer {
class FileSystem;

class ResFilePath {
 public:
  ResFilePath(const char* path);
  ResFilePath(const StringType& path) : path_(path) {}
  ResFilePath(const ResFilePath& path) : path_(path.value()) {}

  ResFilePath& operator = (const ResFilePath& path) {
    path_ = path.value();
    return *this;
  }

  const StringType& value() const { return path_;}
  ResFilePath AppendCopy(const StringType& path) const;
  void Append(const StringType& str);
  bool empty() const { return path_.empty();}
 protected:
  StringType path_;
  static const CharType kSeperator;
};

class AZER_EXPORT FileSystem {
 public:
  virtual ~FileSystem() {}

  enum Type {
    kNativeFS = 1,
    kPackaged,
    kCompressedPackaged,
  };

  static void InitDefaultFileSystem(const ::base::FilePath& root,
                                    Type type = kNativeFS);
  static void InitDefaultFileSystem(const StringType& root,Type type = kNativeFS);
  static FileSystem* GetDefaultFileSystem();

  static FileSystem* create(Type type, const ::base::FilePath& root);
  const ::base::FilePath& root() { return fs_root_;}

  virtual FileContentPtr LoadFile(const ResFilePath& path) = 0;
  virtual bool IsPathExists(const ResFilePath& path) = 0;
 protected:
  FileSystem(Type type, const ::base::FilePath& root)
      : type_(type)
      , fs_root_(root) {}

  const Type type_;
  const ::base::FilePath fs_root_;

  friend struct ::base::DefaultLazyInstanceTraits<FileSystem>;
  DISALLOW_COPY_AND_ASSIGN(FileSystem);
};

typedef std::shared_ptr<FileSystem> FileSystemPtr;

inline ResFilePath ResFilePath::AppendCopy(const StringType& path) const {
  ResFilePath new_path(path_);
  new_path.Append(path);
  return new_path;
}

inline void ResFilePath::Append(const StringType& str) {
  if (path_.back() == kSeperator) {
    path_.append(str);
  } else {
    path_.push_back(kSeperator);
    path_.append(str);
  }
}

inline void FileSystem::InitDefaultFileSystem(const StringType& root, Type type) {
  InitDefaultFileSystem(::base::FilePath(root), type);
}

inline bool SplitPackage(const StringType& full, StringType* path,
                         StringType* package) {
  size_t pos = full.find_last_of(':');
  if (pos < 0) return false;
  path->assign(full.substr(0, pos));
  package->assign(full.substr(pos + 1));
  return true;
}

}  // namespace azer
