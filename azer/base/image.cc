#include "azer/base/image.h"

#include "base/files/file_path.h"
#include "azer/render/util/image.h"

namespace azer {
Image* Image::Load(const ::base::FilePath& path) {
  return LoadImageFromFile(path);
}

bool Image::Save(const ::base::FilePath& path) {
  return SaveImage(this, path);
}

}  // namespace azer
