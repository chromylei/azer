#pragma once

#include <memory>

#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/math/math.h"
#include "azer/base/align.h"
#include "azer/base/render_export.h"

namespace azer {
class AZER_EXPORT Light {
 public:
  enum Type {
    kDirectionalLight = 1,
    kPointLight,
    kSpotLight,
    kProjectiveTexLight,
  };

  struct Attenuation {
    float cvalue;
    float linear;
    float expr;

    Attenuation(): cvalue(1.0), linear(1.0), expr(1.0) {}
    Attenuation(float c, float l, float e): cvalue(c), linear(l), expr(e) {}
  };

  struct AZER_GPU_VAR DirectionalLight {
    Vector4 ambient;
    Vector4 diffuse;
    Vector4 directional;
  };
  
  struct AZER_GPU_VAR PointLight {
    Vector4 ambient;
    Vector4 diffuse;
    Vector4 position;
    Attenuation attenuation;
    float range;

    PointLight() : range(0.0) {}
  };

  struct AZER_GPU_VAR SpotLight {
    Vector4 ambient;
    Vector4 diffuse;
    Vector4 position;
    Vector4 directional;
    
    Attenuation attenuation;
    float range;
    Radians cone;

    SpotLight() : range(0.0) {}
  };

  Light(Type type)
      : type_(type) {
  }

  Light(const DirectionalLight& light)
      : dir_light_(light)
      , type_(kDirectionalLight) {}
  Light(const PointLight& light)
      : point_light_(light)
      , type_(kPointLight) {}
  Light(const SpotLight& light)
      : spot_light_(light)
      , type_(kSpotLight) {}

  virtual ~Light() {}

  const Vector4& directional() const;
  void set_directional(const Vector4& dir);

  const Vector4& position() const;
  void set_position(const Vector4& pos);

  const Vector4& ambient() const;
  void set_ambient(const Vector4& amb);
  
  const Vector4& diffuse() const;
  void set_diffuse(const Vector4& diffuse);

  int32 gpu_exchange_size() const { return gpu_exchange_size(type_);}
  static int32 gpu_exchange_size(Type type);

  void set_attenuation(const Attenuation& att);
  const Attenuation& attenuation() const;

  void set_range(float r);
  float range() const;
  void set_cone(float c);
  float cone() const;

  Type type() const { return type_;}
 protected:
  DirectionalLight dir_light_;
  PointLight point_light_;
  SpotLight spot_light_;
  
  const Type type_;

  // access data by GpuContantsTable
  const uint8* gpu_constant_data() const {
    package();
    return package_;
  }

  int32 package() const;

  static const int kMaxLightMemorySize = sizeof(Vector4) * 8;
  mutable uint8 package_[kMaxLightMemorySize];
  friend class GpuConstantsTable;
  DISALLOW_COPY_AND_ASSIGN(Light);
};

typedef std::shared_ptr<Light> LightPtr;

inline int32 Light::gpu_exchange_size(Type type) {
  switch (type) {
    case kDirectionalLight:
      return sizeof(DirectionalLight);
    case kPointLight:
      return sizeof(PointLight);
    case kSpotLight:
      return sizeof(SpotLight);
    default:
      CHECK(false); return 0;
  }
}

inline void Light::set_directional(const Vector4& dir) {
  if(type() == kDirectionalLight) {
    dir_light_.directional = dir;
  } else if (type() == kSpotLight) {
    spot_light_.directional = dir;
  } else {
    DCHECK(false);
  }
}

inline const Vector4& Light::directional() const {
  if(type() == kDirectionalLight) {
    return dir_light_.directional;
  } else if (type() == kSpotLight) {
    return spot_light_.directional;
  } else {
    CHECK(false);
    return dir_light_.directional;
  }
}

inline void Light::set_position(const Vector4& dir) {
  if(type() == kPointLight) {
    point_light_.position = dir;
  } else if (type() == kSpotLight) {
    spot_light_.position = dir;
  } else {
    DCHECK(false);
  }
}

inline const Vector4& Light::position() const {
  if(type() == kPointLight) {
    return point_light_.position;
  } else if (type() == kSpotLight) {
    return spot_light_.position;
  } else {
    DCHECK(false);
    return point_light_.position;
  }
}

inline const Vector4& Light::ambient() const {
  if (type() == kDirectionalLight) {
    return dir_light_.ambient;
  } else if(type() == kPointLight) {
    return point_light_.ambient;
  } else if (type() == kSpotLight) {
    return spot_light_.ambient;
  } else {
    CHECK(false);
    return dir_light_.ambient;
  }
}

inline void Light::set_ambient(const Vector4& amb) {
  if (type() == kDirectionalLight) {
    dir_light_.ambient = amb;
  } else if(type() == kPointLight) {
    point_light_.ambient = amb;
  } else if (type() == kSpotLight) {
    spot_light_.ambient = amb;
  } else {
    CHECK(false);
  }
}
  
inline const Vector4& Light::diffuse() const {
  if (type() == kDirectionalLight) {
    return dir_light_.diffuse;
  } else if(type() == kPointLight) {
    return point_light_.diffuse;
  } else if (type() == kSpotLight) {
    return spot_light_.diffuse;
  } else {
    CHECK(false);
    return dir_light_.diffuse;
  }
}

inline void Light::set_diffuse(const Vector4& diffuse) {
  if (type() == kDirectionalLight) {
    dir_light_.diffuse = diffuse;
  } else if(type() == kPointLight) {
    point_light_.diffuse = diffuse;
  } else if (type() == kSpotLight) {
    spot_light_.diffuse = diffuse;
  } else {
    CHECK(false);
  }
}

inline void Light::set_attenuation(const Attenuation& att) {
  if(type() == kPointLight) {
    point_light_.attenuation = att;
  } else if (type() == kSpotLight) {
    spot_light_.attenuation = att;
  } else {
    DCHECK(false);
  }
}

inline const Light::Attenuation& Light::attenuation() const {
  if(type() == kPointLight) {
    return point_light_.attenuation;
  } else if (type() == kSpotLight) {
    return spot_light_.attenuation;
  } else {
    DCHECK(false);
    return point_light_.attenuation;
  }
}

inline void Light::set_range(float r) {
  if(type() == kPointLight) {
    point_light_.range = r;
  } else if (type() == kSpotLight) {
    spot_light_.range = r;
  } else {
    DCHECK(false);
  }
}

inline float Light::range() const {
  if(type() == kPointLight) {
    return point_light_.range;
  } else if (type() == kSpotLight) {
    return spot_light_.range;
  } else {
    DCHECK(false);
    return 0.0f;
  }
}

inline void Light::set_cone(float c) {
  DCHECK(type() == kSpotLight);
  spot_light_.cone = c;
}

inline float Light::cone() const {
  DCHECK(type() == kSpotLight);
  return spot_light_.cone;
}

}  // namespace azer
