#pragma once


namespace azer {
namespace math {

// compute the reflection directional of light
Vector4 LightReflectDirection(const Vector4& light, const Vector4& normal);
Vector3 LightReflectDirection(const Vector3& light, const Vector3& normal);
}  // namespace math
}  // namespace azer
