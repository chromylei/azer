#pragma once

namespace azer {
class Radians;
class Vector3;
class Vector4;

Matrix4 Translate(float x, float y, float z);
Matrix4 Translate(const Vector3& pos);

Matrix4 RotateX(Radians r);
Matrix4 RotateX(Degree r);

Matrix4 RotateY(Radians r);
Matrix4 RotateY(Degree r);

Matrix4 RotateZ(Radians r);
Matrix4 RotateZ(Degree r);

Matrix4 Scale(float scale);
Matrix4 Scale(float x, float y, float z);
Matrix4 Scale(const Vector3& scale);


/**
 * eye: position of camera
 * lookat: position of camera look at
 * up: direction of camear up
 * Note: lookat is a vector not a position
 */
Matrix4 LookAtRH(const Vector3& eye, const Vector3& lookat, const Vector3& up);
/**
 * 注意： dir 为 camera 对准的方向
 * 在右手坐标系当中，它与 Z 轴的方向是相反的
 */
Matrix4 LookDirRH(const Vector3& eye, const Vector3& dir, const Vector3& up);

Matrix4 LookAtLH(const Vector4& eye, const Vector4& lookat, const Vector4& up);

inline Matrix4 Translate(float x, float y, float z) {
  Matrix4 mat = Matrix4::kIdentity;
  mat[0][3] = x;
  mat[1][3] = y;
  mat[2][3] = z;
  return mat;
}

inline Matrix4 Translate(const Vector3& pos) {
  return Translate(pos.x, pos.y, pos.z);
}

inline Matrix4 Scale(float scale) {
  return Matrix4(scale, 0.0f,  0.0f,  0.0f,
                 0.0f,  scale, 0.0f,  0.0f,
                 0.0f,  0.0f,  scale, 0.0f,
                 0.0f,  0.0f,  0.0f,  1.0f);
}

inline Matrix4 Scale(float x, float y, float z) {
  return Matrix4(x,     0.0f,  0.0f,  0.0f,
                 0.0f,  y,     0.0f,  0.0f,
                 0.0f,  0.0f,  z,     0.0f,
                 0.0f,  0.0f,  0.0f,  1.0f);
}

inline Matrix4 Scale(const Vector3& scale) {
  return Matrix4(scale.x,  0.0f,      0.0f,        0.0f,
                 0.0f,       scale.y, 0.0f,        0.0f,
                 0.0f,       0.0f,      scale.z,   0.0f,
                 0.0f,       0.0f,      0.0f,      1.0f);
}

inline Matrix4 RotateX(Degree d) { return RotateX(Radians(d));}
inline Matrix4 RotateX(Radians r) {
  float s = sin(r);
  float c = cos(r);
  return Matrix4(1.0f,  0.0f,  0.0f,  0.0f,
                 0.0f,     c,     s,  0.0f,
                 0.0f,    -s,     c,  0.0f,
                 0.0f,  0.0f,  0.0f,  1.0f);
}

inline Matrix4 RotateY(Degree d) { return RotateY(Radians(d));}
inline Matrix4 RotateY(Radians r) {
  float s = sin(r);
  float c = cos(r);
  return Matrix4(c,     0.0f,    -s,  0.0f,
                 0.0f,  1.0f,  0.0f,  0.0f,
                 s,    0.0f,      c,  0.0f,
                 0.0f,  0.0f,  0.0f,  1.0f);
}

inline Matrix4 RotateZ(Degree d) { return RotateZ(Radians(d));}
inline Matrix4 RotateZ(Radians r) {
  float s = sin(r);
  float c = cos(r);
  return Matrix4(   c,    s,  0.0f,  0.0f,
                    -s,    c,  0.0f,  0.0f,
                 0.0f,  0.0f,  1.0f,  0.0f,
                 0.0f,  0.0f,  0.0f,  1.0f);
}

inline Matrix4 LookAtRH(const Vector3& eye, const Vector3& lookat,
                        const Vector3& up) {
  Vector3 camera_z = (eye - lookat).NormalizeCopy();
  return LookDirRH(eye, -camera_z, up);
}
}  // namespace azer
