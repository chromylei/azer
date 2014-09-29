#include "azer/base/image.h"

#include <cmath>

#include "base/files/file_path.h"
#include "azer/render/util/image.h"

namespace azer {
ImageData* ImageData::Load(const ::base::FilePath& path) {
  return LoadImageData(path);
}

Image* Image::Load(const ::base::FilePath& path) {
  return LoadImageFromFile(path);
}

Vector4 ImageData::BoxSample(float tu, float tv) const {
  float x = width() * tu;
  float y = width() * tv;
  int x1 = std::max(std::floor(x), 0.0f);
  int x2 = std::min(std::ceil(x), width() - 1.0f);
  int y1 = std::max(std::floor(y), 0.0f);
  int y2 = std::min(std::ceil(y), width() - 1.0f);
  uint32 c1 = pixel(x1, y1);
  uint32 c2 = pixel(x1, y2);
  uint32 c3 = pixel(x2, y1);
  uint32 c4 = pixel(x2, y2);
  azer::Vector4 color;
  color.w = (((c1 & 0xFF000000) >> 24) + ((c2 & 0xFF000000) >> 24)
             + ((c3 & 0xFF000000) >> 24) + ((c4 & 0xFF000000) >> 24));
  color.z = ((c1 & 0x00FF0000) + (c2 & 0x00FF0000) + (c3 & 0x00FF0000) +
             (c4 & 0x00FF0000)) >> 16;
  color.y = ((c1 & 0x0000FF00) + (c2 & 0x0000FF00) + (c3 & 0x0000FF00) +
             (c4 & 0x0000FF00)) >> 8;
  color.x = ((c1 & 0x000000FF) + (c2 & 0x000000FF) + (c3 & 0x000000FF) +
             (c4 & 0x000000FF));
  return color * 0.25f / 255.0f;
}
}  // namespace azer
