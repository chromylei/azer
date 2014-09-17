#include "azer/render/texture.h"
#include "azer/render/util/image.h"

namespace azer {
Texture* Texture::CreateShaderTexture(const ::base::FilePath& path,
                                      azer::RenderSystem* rs) {
  return azer::CreateShaderTexture(path, rs);
}
}   // namespace azer
