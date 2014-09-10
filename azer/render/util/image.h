#pragma once

#include <memory>
#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "azer/render/render.h"
#include "azer/base/image.h"

namespace azer {
azer::Vector4 AZER_EXPORT SampleImage(float u, float v, Image* image);

// load image from file
azer::ImagePtr AZER_EXPORT LoadImageFromFile(const ::base::FilePath& path);

inline azer::ImagePtr LoadImageFromFile(const ::base::FilePath::StringType& path) {
  return LoadImageFromFile(::base::FilePath(path));
}

bool AZER_EXPORT SaveImage(azer::Image* image, const ::base::FilePath& path);
}  // namespace azer
