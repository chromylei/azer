#include "azer/math/math.h"
#include "base/logging.h"

namespace azer {
/*
 * 根据光线的入射方向和法线方向计算计算光线的反射防线
 * 设 光线的入射方向为 L, 法线方向为 N， 反射方向为 R
 * R = -2 N (L . N) + L
 * 从向量的加法可以非常直观的理解公式的含义
 */
Vector4 LightReflectDirection(const Vector4& light, const Vector4& normal) {
  return Vector4();
}

}  // namespace azer
