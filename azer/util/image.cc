#include "azer/util/image.h"

#include "base/file_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/logging.h"
#include "azer/util/ilimage_wrapper.h"
#include "azer/base/image.h"

namespace azer {
namespace util {

Image::Image()
    : image_(NULL) {
}

Image::~Image() {
  if (image_) {
    delete image_;
  }
}

uint32 Image::GetData(int x, int y) {
  DCHECK(image_ != NULL);
  return image_->GetData(x, y);
}

bool Image::Load(const ::base::FilePath& path) {
  DCHECK(image_ == NULL);
  image_ = new detail::ilImageWrapper;
  return image_->Load(path);
}

int Image::height() const {
  DCHECK(image_ != NULL);
  return image_->height();
}
int Image::width() const {
  DCHECK(image_ != NULL);
  return image_->width();
}

uint8* Image::GetDataPtr() {
  DCHECK(image_ != NULL);
  return image_->GetDataPtr();
}

uint32 Image::GetDataSize() {
  DCHECK(image_ != NULL);
  return image_->GetDataSize();
}

azer::Vector4 SampleImage(float u, float v, Image* image) {
  int x = u * image->width();
  int y = v * image->height();
  int32 rgba = image->GetData(x, y);
  float r = (float)((rgba & 0xFF000000) >> 24) / 255.0f;
  float g = (float)((rgba & 0x00FF0000) >> 16) / 255.0f;
  float b = (float)((rgba & 0x0000FF00) >> 8) / 255.0f;
  float a = (float)(rgba & 0x000000FF) / 255.0f;
  return azer::Vector4(r, g, b, 1.0f);
}

TexturePtr CreateTextureForShader(Image* image, RenderSystem* rs) {
  azer::Texture::Options texopt;
  texopt.width = image->width();
  texopt.height = image->height();
  texopt.target = azer::Texture::kShaderResource;
  azer::TexturePtr ptr(rs->CreateTexture(texopt));
  if (ptr->InitFromData(image->GetDataPtr(), image->GetDataSize())) {
    return ptr;
  } else {
    return azer::TexturePtr();
  }
}

bool SaveImage(azer::Image* image, const ::base::FilePath& path) {
  detail::ilImageWrapper ilimg;
  ilimg.Create(image->width(), image->height());
  ilimg.InitFromData(image->data());
  return ilimg.Save(path);
}

}  // namespace util
}  // namespace azer
