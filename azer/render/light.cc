#include "azer/render/light.h"

namespace azer {
int32 Light::package() const {
  // copy common part
  uint8* cur = package_;
  switch (type_) {
    case kDirectionalLight:
      memcpy(cur, &dir_light_, sizeof(dir_light_));
      break;
    case kPointLight:
      memcpy(cur, &point_light_, sizeof(point_light_));
      break;
    case kSpotLight:
      memcpy(cur, &spot_light_, sizeof(spot_light_));
      break;
    default:
      CHECK(false);
      break;
  }
  return cur - package_;
}
}  // namespace azer
