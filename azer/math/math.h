#pragma once

#include <cmath>
#include <limits>
#include <iomanip>
#include "base/logging.h"

namespace azer {

inline float sqrt(float v) {
  return std::sqrt(v);
}

inline float inv_sqrt(float v) {
  return 1.0f / std::sqrt(v);
}

inline float sqr(float v) {
  return v * v;
}

inline float acos(float v) {
  return std::acos(v);
}

inline float atan2(float x, float y) {
  return std::atan2(x, y);
}

inline float isnan(float x) {
  return x != x;
}

}  // namespace azer


#define INCLUDED_BY_MATH
#include "azer/math/consts.h"
#include "azer/math/degree.h"
#include "azer/math/vector2.h"
#include "azer/math/vector3.h"
#include "azer/math/vector4.h"
#include "azer/math/intvec.h"
#include "azer/math/matrix3.h"
#include "azer/math/matrix4.h"
#include "azer/math/plane.h"
#include "azer/math/transform.h"
#include "azer/math/projection.h"
#include "azer/math/light_model.h"
#include "azer/math/quaternion.h"
#include "azer/math/float_util.h"
#include "azer/math/ray.h"
#include "azer/math/logging.h"

#include "azer/math/matrix4_inl.h"
#include "azer/math/vector3_inl.h"
#include "azer/math/vector4_inl.h"
#include "azer/math/quaternion_inl.h"
#include "azer/math/normal_util.h"
#include "azer/math/plane_inl.h"

namespace azer {
typedef Vector2 vec2;
typedef Vector3 vec3;
typedef Vector4 vec4;
typedef Matrix4 mat4;
}  // namespace azer
#undef INCLUDED_BY_MATH



