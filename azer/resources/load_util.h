#pragma once

#include "base/basictypes.h"
#include "azer/base/render_export.h"
#include "azer/render/material.h"
#include "azer/render/texture.h"
#include "azer/render/mesh.h"

namespace azer {

TexturePtr AZER_EXPORT LoadTexture(const base::FilePath& path);
MeshPtr AZER_EXPORT LoadMesh(const base::FilePath& path);

// load animation
// load skeletion
// load scene
// load lighting
}  // namespace azer
