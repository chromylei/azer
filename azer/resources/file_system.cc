#include "azer/resources/file_system.h"

#include "base/logging.h"
#include "base/file_util.h"
#include "base/files/file_path.h"
#include "base/strings/utf_string_conversions.h"

namespace azer {

const CharType ResFilePath::kSeperator = FILE_PATH_LITERAL('/');

class NativeFileSystem : public FileSystem {
 public:
  NativeFileSystem(const ::base::FilePath& root)
      : FileSystem(FileSystem::kNativeFS, root) {
  }

  virtual FileContentPtr LoadFile(const ResFilePath& path) OVERRIDE {
    DCHECK(!path.empty());
    ::base::FilePath real_path = fs_root_.Append(path.value());
    int64 size = 0;
    if (!::base::GetFileSize(real_path, &size)) {
      return NULL;
    }

    std::unique_ptr<char[]> content(new char[size - 1 + sizeof(FileContent)]);
    if (size != ::base::ReadFile(real_path,
                                 content.get() + offsetof(FileContent, data),
                                 size)) {
      return NULL;
    }

    FileContentPtr cptr((FileContent*)content.release());
    cptr->length = size;
    return cptr;
  }

  virtual bool IsPathExists(const ResFilePath& path) OVERRIDE {
    DCHECK(!path.empty());
    ::base::FilePath real_path = fs_root_.Append(path.value());
    return base::PathExists(real_path);
  }
 private:
  DISALLOW_COPY_AND_ASSIGN(NativeFileSystem);
};

FileSystem* FileSystem::create(FileSystem::Type type, const ::base::FilePath& root) {
  switch(type) {
    case kNativeFS:
      return new NativeFileSystem(root); 
    default:
      NOTREACHED();
      return NULL;
  }
}

ResFilePath::ResFilePath(const char* path) {
  ConvertPath<StringType>(path, &path_);
}

namespace {
FileSystem* comm_res_fs_ = NULL;
}  // namespace

void FileSystem::InitDefaultFileSystem(const ::base::FilePath& root, Type type) {
  CHECK(comm_res_fs_ == NULL);
  comm_res_fs_ = create(type, root);
}

FileSystem* FileSystem::GetDefaultFileSystem() {
  CHECK(comm_res_fs_ != NULL);
  return comm_res_fs_;
}
}  // namespace azer
