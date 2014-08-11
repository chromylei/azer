#include "azer/math/matrix4.h"
#include "azer/math/projection.h"
#include "base/logging.h"


namespace azer {

Matrix4 PerspectiveRH(Radians fovy, float aspect, float z_near, float z_far) {
  DCHECK_GT(aspect, 1.0f);
  Matrix4 mat = Matrix4::kIdentity;
  const float tan_half_fov = tan(fovy / 2.0f);
  const float z_range = (z_far - z_near);
  float yscale = 1.0f / tan_half_fov;
  float xscale = yscale / aspect;

  mat[0][0] = xscale;
  mat[1][1] = yscale;
  mat[2][2] = -(z_far + z_near) / z_range;
  mat[2][3] = -2.0f * z_far * z_near / z_range;
  
  mat[3][2] = -1.0f;
  mat[3][3] = 0.0f;
  return mat;
}

Matrix4 PerspectiveRHD3D(Radians fovy, float aspect, float z_near, float z_far) {
  const float tan_half_fov = tan(fovy / 2.0f);
  const float z_range = (z_far - z_near);
  float yscale = 1 / tan_half_fov;
  float xscale = yscale / aspect;

  return Matrix4(xscale,  0.0f,              0.0f,                      0.0f,
                 0.0f,  yscale,              0.0f,                      0.0f,
                 0.0f,    0.0f,  -z_far / z_range, -z_near * z_far / z_range,
                 0.0f,    0.0f,             -1.0f,                      0.0f);
}

Matrix4 OrthoProjRH(float width, float height, float z_near, float z_far) {
  Matrix4 mat = Matrix4::kIdentity;
  const float z_range = (z_far - z_near);
  return Matrix4(0.5f * width, 0.0f,          0.0f,              0.0f,
                 0.0f,         0.5f * height, 0.0f,              0.0f,
                 0.0f,         0.0f,          1.0f / z_range,    z_near / z_range,
                 0.0f,         0.0f,          0.0f,              1.0f);
}
}  // namespace azer
