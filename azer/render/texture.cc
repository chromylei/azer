#include "azer/render/texture.h"
#include "azer/render/util/image.h"

namespace azer {
Texture* Texture::LoadShaderTexture(const ::base::FilePath& path,
                                    azer::RenderSystem* rs) {
  Texture::Options texopt;
  texopt.target = Texture::kShaderResource;
  std::unique_ptr<Image> imgptr(LoadImageFromFile(path));
  if (!imgptr.get()) {
    return NULL;
  }

  texopt.type = (Texture::Type)imgptr->type();
  return rs->CreateTexture(texopt, imgptr.get());
}

Texture* Texture::LoadTexture(const Texture::Options& opt,
                              const ::base::FilePath& path,
                              RenderSystem* rs) {
  std::unique_ptr<Image> imgptr(LoadImageFromFile(path));
  if (imgptr.get()) {
    return rs->CreateTexture(opt, imgptr.get());
  } else {
    LOG(ERROR) << "failed to load texture: \"" << path.value() << "\"";
    return NULL;
  }
}
}   // namespace azer
