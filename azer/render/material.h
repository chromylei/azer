#pragma once

#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/base/align.h"
#include "azer/base/render_export.h"
#include "azer/math/math.h"
#include "azer/render/texture.h"

namespace azer {
class AZER_EXPORT Material {
 public:
  Material() {}
  virtual ~Material() {}
  void Use();

  struct AZER_GPU_VAR material {
    float transparency;
    float specular_power;
    float optical_density;  // 0~10
    int illumination;
    azer::vec4 ambient;
    azer::vec4 diffuse;
    azer::vec4 specular_color;
    azer::vec4 emissive_color;
    material()
        : transparency(1.0)
        , specular_power(0.0)
        , optical_density(0.0)
        , illumination(0) {
    }
  };

  void AddDiffuseTex(TexturePtr ptr) {
    DCHECK(NULL != ptr.get());
    diffuse_tex_.push_back(ptr);
  }

  void AddSpecularTex(TexturePtr ptr) {
    DCHECK(NULL != ptr.get());
    specular_tex_.push_back(ptr);
  }

  void AddNormalTex(TexturePtr ptr) {
    DCHECK(NULL != ptr.get());
    normal_tex_.push_back(ptr);
  }

  void AddAlphaTex(TexturePtr ptr) {
    DCHECK(NULL != ptr.get());
    alpha_tex_.push_back(ptr);
  }

  material& attributes() { return material_;}
  const material& attributes() const { return material_;}

  typedef std::vector<TexturePtr> TextureVec;
  const TextureVec& diffuse_tex() const { return diffuse_tex_;}
  const TextureVec& specular_tex() const { return specular_tex_;}
  const TextureVec& normal_tex() const { return normal_tex_;}
  const TextureVec& alpha_tex() const { return alpha_tex_;}
 protected:
  material material_;
  TextureVec diffuse_tex_;
  TextureVec specular_tex_;
  TextureVec normal_tex_;
  TextureVec alpha_tex_;
  DISALLOW_COPY_AND_ASSIGN(Material);
};

typedef std::shared_ptr<Material> MaterialPtr;
}  // namespace azer
