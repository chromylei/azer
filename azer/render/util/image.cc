#include "azer/render/util/image.h"

#include "base/file_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/logging.h"
#include "azer/render/util/ilimage_wrapper.h"
#include "azer/base/image.h"
#include "azer/base/string.h"

namespace azer {

azer::Vector4 SampleImage(float u, float v, Image* img) {
  ImageDataPtr image = img->data(0);
  int x = u * image->width();
  int y = v * image->height();
  int32 rgba = image->pixel(x, y);
  float r = (float)((rgba & 0xFF000000) >> 24) / 255.0f;
  float g = (float)((rgba & 0x00FF0000) >> 16) / 255.0f;
  float b = (float)((rgba & 0x0000FF00) >> 8) / 255.0f;
  float a = (float)(rgba & 0x000000FF) / 255.0f;
  return azer::Vector4(r, g, b, 1.0f);
}

bool SaveImage(azer::ImageData* image, const ::base::FilePath& path) {
  detail::ilImageWrapper ilimg;
  ilimg.Create(image->width(), image->height());
  ilimg.InitFromData(image->data());
  return ilimg.Save(path);
}

azer::Image* LoadImageFromFile(const ::base::FilePath& path) {
  ImageDataPtr ptr(LoadImageData(path));
  if (ptr.get() != NULL) {
    std::vector<ImageDataPtr> vec;
    vec.push_back(ptr);
    return new Image(vec);
  } else {
    return NULL;
  }
}

ImageData* LoadImageData(const ::base::FilePath& path) {
  detail::ilImageWrapper ilimg;
  if (!ilimg.Load(path)) {
    return NULL;
  }

  int32 size = ilimg.GetDataSize();
  std::unique_ptr<ImageData> ptr(
      new ImageData(ilimg.width(), ilimg.height(), kRGBAn8));
  ilimg.CopyToImage(ptr.get());
  return ptr.release();
}

Texture* CreateShaderTexture(const ::base::FilePath& path, azer::RenderSystem* rs) {
  Texture::Options texopt;
  texopt.target = Texture::kShaderResource;
  std::unique_ptr<Image> imgptr(LoadImageFromFile(path));
  if (imgptr.get()) {
    return rs->CreateTexture(texopt, imgptr.get());
  } else {
    LOG(ERROR) << "failed to load texture: \"" << path.value() << "\"";
    return NULL;
  }
}
}  // namespace azer
