#pragma once

#include <memory>
#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "azer/render/render.h"
#include "azer/base/image.h"

namespace azer {
AZER_EXPORT Vector4 SampleImage(float u, float v, Image* image);

// load image from file
AZER_EXPORT ImagePtr LoadImageFromFile(const ::base::FilePath& path);

inline ImagePtr LoadImageFromFile(const ::base::FilePath::StringType& path) {
  return LoadImageFromFile(::base::FilePath(path));
}

// create texture for shader resource
AZER_EXPORT Texture* CreateShaderTexture(const ::base::FilePath& path,
                                         azer::RenderSystem* rs);
inline Texture* CreateShaderTexture(const ::base::FilePath::StringType& path,
                                    azer::RenderSystem* rs) {
  return CreateShaderTexture(::base::FilePath(path), rs);
}

bool AZER_EXPORT SaveImage(Image* image, const ::base::FilePath& path);
}  // namespace azer
