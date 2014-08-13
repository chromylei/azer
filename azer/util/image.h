#pragma once

#include <memory>
#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "azer/render/render.h"

namespace azer {
namespace util {
namespace detail {
class ilImageWrapper;
}  // namespace detail

class Image {
 public:
  Image();
  ~Image();

  int height() const;
  int width() const;

  uint8* GetDataPtr();
  uint32 GetData(int x, int y);
  bool Load(const ::base::FilePath& path);
  uint32 GetDataSize();
 private:
  detail::ilImageWrapper* image_;
  DISALLOW_COPY_AND_ASSIGN(Image);
};

TexturePtr CreateTextureForShader(Image* image, RenderSystem* rs);
azer::Vector4 SampleImage(float u, float v, Image* image);

}  // namespace util
}  // namespace azer
