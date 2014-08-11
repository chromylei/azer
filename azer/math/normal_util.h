#pragma once

namespace azer {

Vector3 CalcPlaneNormal(const Vector3* points);
Vector3 CalcPlaneNormal(const Vector4* points);
Vector3 CalcPlaneNormal(const Vector3& p1, const Vector3& p2, const Vector3& p3);
Vector3 CalcPlaneNormal(const Vector4& p1, const Vector4& p2, const Vector4& p3);

Vector3 CalcTangent(const Vector3& p1, const Vector2& tex1,
                    const Vector3& p2, const Vector2& tex2,
                    const Vector3& p3, const Vector2& tex3);

void CalcTangentAndBinormal(const Vector3& p1, const Vector2& tex1,
                            const Vector3& p2, const Vector2& tex2,
                            const Vector3& p3, const Vector2& tex3,
                            Vector3* tangent, Vector3* binormal);

/**
 * 计算平面的 “法向量“
 */
inline Vector3 CalcPlaneNormal(const Vector3& p1, const Vector3& p2,
                               const Vector3& p3) {
  Vector3 v1 = p2 - p1;
  Vector3 v2 = p3 - p1;
  return CrossProduct(v1, v2).Normalize();
}

inline Vector3 CalcPlaneNormal(const Vector4& p1, const Vector4& p2,
                               const Vector4& p3) {
  Vector4 v1 = p2 - p1;
  Vector4 v2 = p3 - p1;
  return CrossProduct(Vector3(v1), Vector3(v2)).Normalize();
}

inline Vector3 CalcPlaneNormal(const Vector3* points) {
  return std::move(CalcPlaneNormal(points[0], points[1], points[2]));
}
inline Vector3 CalcPlaneNormal(const Vector4* points) {
  return std::move(CalcPlaneNormal(points[0], points[1], points[2]));
}

}  // namespace azer
