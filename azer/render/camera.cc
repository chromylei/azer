#include "azer/render/camera.h"
#include "azer/render/render.h"
#include "azer/math/math.h"

#include <iostream>

namespace azer {

Camera::Camera() : frustrum_(this) {
  Update();
}

Camera::Camera(const Frustrum& frustrum)
    : frustrum_(this, frustrum.fovy(), frustrum.aspect(), frustrum.get_near(),
                frustrum.get_far()) {
  Update();
}

Camera::Camera(const Vector3& pos)
    : MovableObject(pos)
    , frustrum_(this) {
  Update();
}

Camera::Camera(const Vector3& pos, const Vector3& lookat,
               const Vector3& up)
    : frustrum_(this) {
  reset(pos, lookat, up);
}


void Camera::reset(const Vector3& pos, const Vector3& lookat, const Vector3& up) {
  Matrix4 mat = LookAtRH(pos, lookat, up);
  SetPosition(pos);
  Vector3 xaxis(mat[0][0], mat[0][1], mat[0][2]);
  Vector3 yaxis(mat[1][0], mat[1][1], mat[1][2]);
  Vector3 zaxis(mat[2][0], mat[2][1], mat[2][2]);
  orientation_ = azer::Quaternion::FromAxis(xaxis, yaxis, zaxis);
  Update();
}

void Camera::GenMatrices() {

  // TODO
  // View matrix is:
  //
  //  [ Lx  Uy  Dz  Tx  ]
  //  [ Lx  Uy  Dz  Ty  ]
  //  [ Lx  Uy  Dz  Tz  ]
  //  [ 0   0   0   1   ]
  //
  // Where T = -(Transposed(Rot) * Pos)
  /*
  Matrix4 view;
  Matrix4 rotT = std::move(orientation_.ToMatrix());
  Vector4 trans = std::move(-rotT * Vector4(pos_, 1.0f));
  view = rotT;
  view[0][3] = trans.x;
  view[1][3] = trans.y;
  view[2][3] = trans.z;
  */
  view_mat_ = std::move(LookDirRH(position(), direction(), up()));
  proj_view_mat_ = std::move(frustrum_.projection() * view_mat_);
}


void Camera::SetLookAt(const Vector3& lookat) {
  reset(position_, lookat, up());
}

void Camera::SetDirection(const Vector3& dir) {
  // do nothing
  if (dir.length() == 0) { return;}

  // adjust for Right hand
  DCHECK_FLOAT_EQ(dir.length(), 1.0f);
  Vector3 zaxis = -dir;
  CHECK(false);
}

}  // namespace azer
