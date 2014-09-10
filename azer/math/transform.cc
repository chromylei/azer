#include "azer/math/math.h"

namespace azer {

/*
 *  右手坐标系的 LookAt 矩阵
 *  需要非常注意的是，右手坐标系矩阵的 Z 轴与可视方向是相反的
 *  即 z 轴的方向是 position - target 
 */
// right handle
// zaxis = normal(cameraPosition - cameraTarget)
// xaxis = normal(cross(cameraUpVector, zaxis))
// yaxis = cross(zaxis, xaxis)
// xaxis.x           yaxis.x           zaxis.x          -dot(xaxis, cameraPosition)
// xaxis.y           yaxis.y           zaxis.y          -dot(yaxis, cameraPosition)
// xaxis.z           yaxis.z           zaxis.z          -dot(zaxis, cameraPosition)
// 0                       0                 0                                    1

Matrix4 LookDirRH(const Vector3& eye, const Vector3& dir, const Vector3& up) {
  Vector3 camera_z = -dir.NormalizeCopy();
  Vector3 up_normed = std::move(up.NormalizeCopy());
  Vector3 camera_x = std::move(up_normed.cross(camera_z).NormalizeCopy());
  Vector3 camera_y = std::move(camera_z.cross(camera_x).NormalizeCopy());

  Matrix4 mat = Matrix4::kIdentity;
  mat[0][0] = camera_x.x;
  mat[0][1] = camera_x.y;
  mat[0][2] = camera_x.z;
  
  mat[1][0] = camera_y.x;
  mat[1][1] = camera_y.y;
  mat[1][2] = camera_y.z;
  
  mat[2][0] = camera_z.x;
  mat[2][1] = camera_z.y;
  mat[2][2] = camera_z.z;
  
  mat[0][3] = -camera_x.dot(eye);
  mat[1][3] = -camera_y.dot(eye);
  mat[2][3] = -camera_z.dot(eye);
  return mat;
}

/*
 *  左手坐标系的 LookAt 矩阵
 *  需要非常注意的是，右手坐标系矩阵的 Z 轴与可视方向是相反的
 *  即 z 轴的方向是 position - target 
 */
// zaxis = normal(cameraTarget - cameraPosition)
// xaxis = normal(cross(cameraUpVector, zaxis))
// yaxis = cross(zaxis, xaxis)
// xaxis.x           yaxis.x           zaxis.x          -dot(xaxis, cameraPosition)
// xaxis.y           yaxis.y           zaxis.y          -dot(yaxis, cameraPosition)
// xaxis.z           yaxis.z           zaxis.z          -dot(zaxis, cameraPosition)
// 0                       0                 0                                    1
Matrix4 LookAtLH(const Vector3& eye, const Vector3& lookat, const Vector3& up) {
  Vector3 camera_z = (lookat - eye).NormalizeCopy();
  Vector3 up_normed = std::move(up.NormalizeCopy());
  Vector3 camera_x = std::move(up_normed.cross(camera_z).NormalizeCopy());
  Vector3 camera_y = std::move(camera_z.cross(camera_x).NormalizeCopy());

  Matrix4 mat = Matrix4::kIdentity;
  mat[0][0] = camera_x.x;
  mat[0][1] = camera_x.y;
  mat[0][2] = camera_x.z;
  
  mat[1][0] = camera_y.x;
  mat[1][1] = camera_y.y;
  mat[1][2] = camera_y.z;
  
  mat[2][0] = camera_z.x;
  mat[2][1] = camera_z.y;
  mat[2][2] = camera_z.z;
  
  mat[0][3] = -camera_x.dot(eye);
  mat[1][3] = -camera_y.dot(eye);
  mat[2][3] = -camera_z.dot(eye);
  return mat;
}

#define SQR(x) ((x) * (x))

Matrix4 MirrorTrans(const Plane& plane) {
  // reference http://en.wikipedia.org/wiki/Transformation_matrix
  Matrix4 mat = Matrix4::kIdentity;
  mat[0][0] = 1 - 2 * SQR(plane.normal().x);
  mat[0][1] = -2 * plane.normal().x * plane.normal().y;
  mat[0][2] = -2 * plane.normal().x * plane.normal().z;
  mat[0][3] = -2 * plane.normal().x * plane.d();

  mat[1][0] = -2 * plane.normal().x * plane.normal().y;
  mat[1][1] = 1 - 2 * SQR(plane.normal().y);
  mat[1][2] = -2 * plane.normal().y * plane.normal().z;
  mat[1][3] = -2 * plane.normal().y * plane.d();

  mat[2][0] = -2 * plane.normal().x * plane.normal().z;
  mat[2][1] = -2 * plane.normal().y * plane.normal().z;
  mat[2][2] = 1 - 2 * SQR(plane.normal().z);
  mat[2][3] = -2 * plane.normal().z * plane.d();
  return mat;
}
}  // namespace azer
