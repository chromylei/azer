#include "azer/render/util/image.h"

#include "base/file_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/logging.h"
#include "azer/render/util/ilimage_wrapper.h"
#include "azer/base/image.h"

namespace azer {

azer::Vector4 SampleImage(float u, float v, Image* image) {
  int x = u * image->width();
  int y = v * image->height();
  int32 rgba = image->pixel(x, y);
  float r = (float)((rgba & 0xFF000000) >> 24) / 255.0f;
  float g = (float)((rgba & 0x00FF0000) >> 16) / 255.0f;
  float b = (float)((rgba & 0x0000FF00) >> 8) / 255.0f;
  float a = (float)(rgba & 0x000000FF) / 255.0f;
  return azer::Vector4(r, g, b, 1.0f);
}

bool SaveImage(azer::Image* image, const ::base::FilePath& path) {
  detail::ilImageWrapper ilimg;
  ilimg.Create(image->width(), image->height());
  ilimg.InitFromData(image->data());
  return ilimg.Save(path);
}

azer::ImagePtr LoadImageFromFile(const ::base::FilePath& path) {
  detail::ilImageWrapper ilimg;
  if (!ilimg.Load(path)) {
    return azer::ImagePtr();
  }

  int32 size = ilimg.GetDataSize();
  azer::ImagePtr ptr(new azer::Image(ilimg.width(), ilimg.height(), azer::kRGBAn8));
  ilimg.CopyToImage(ptr.get());
  return ptr;
}

Texture* CreateShaderTexture(const ::base::FilePath& path, azer::RenderSystem* rs) {
  Texture::Options texopt;
  texopt.target = Texture::kShaderResource;
  ImagePtr imgptr(LoadImageFromFile(path));
  return rs->CreateTexture(texopt, imgptr.get());
}
}  // namespace azer
