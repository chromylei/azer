#pragma once


#include "azer/base/render_export.h"
#include "azer/base/movable.h"
#include "azer/math/math.h"
#include "azer/render/frustrum.h"

namespace azer {

/**
 * Camera 的实现思路
 * 默认情况下 Camera 的位置在 (0, 0, 0, 1), 面向 -z 方向
 * Camera 使用两个向量表示 position 和 orientation(Q)
 * 其中 orientation 使用四元数表示，它表示如何将默认的 Camera 旋转至当前方向
 * 因此确定的各轴方式的是 Q * axis * Q^-1, 此运算 Quaternion 提供了直接的方法
 *
 */
class AZER_EXPORT Camera : public MovableObject {
 public:
  Camera();
  Camera(const Frustrum& frustrum);
  explicit Camera(const Vector3& pos);
  Camera(const Vector3& pos, const Vector3& lookat, const Vector3& up);
  void reset(const Vector3& pos, const Vector3& lookat, const Vector3& up);

  void SetLookAt(const Vector3& lookat);
  void SetDirection(const Vector3& dir);
  void SetOrientation(const Quaternion& orientation);

  const Matrix4& GetViewMatrix() const { return view_mat_;}
  const Matrix4& GetProjViewMatrix() const { return proj_view_mat_;}

  void Update() {
    GenMatrices();
    frustrum_.UpdatePlane();
  }
  const Frustrum& frustrum() const { return frustrum_;}
  Frustrum& frustrum() { return frustrum_;}
  friend std::ostream& operator << (std::ostream& os, const Camera& camera);
 private:
  // generate camera transform matrix
  // the matrix will transform the object from world-cord to camera-cord
  // Note: the view matrix is RIGHT-HAND  
  void GenMatrices();

  Matrix4 view_mat_;
  Matrix4 proj_view_mat_;
  Frustrum frustrum_;
  void UpdateDirection();
  DISALLOW_COPY_AND_ASSIGN(Camera);
};

inline std::ostream& operator << (std::ostream& os, const Camera& camera) {
  os << "azer::camera info{ dir:" << camera.direction()
     << ", up: " << camera.up()
     << ", pos: " << camera.position() << "}";
  return os;
}
}  // namespace azer

