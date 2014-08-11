#pragma once

#include "azer/base/render_export.h"
#include "azer/math/math.h"
#include "base/basictypes.h"

namespace azer {
class AZER_EXPORT MovableObject {
 public:
  MovableObject();
  MovableObject(const Vector3& pos);
  MovableObject(const Vector3& pos, const Quaternion& orient);
  MovableObject(const MovableObject& obj);

  MovableObject& operator = (const MovableObject& obj);
  
  void pitch(const Radians angle);
  void yaw(const Radians angle);
  void roll(const Radians angle);

  void pitch(const Degree angle);
  void yaw(const Degree angle);
  void roll(const Degree angle);

  void rotate(const Vector3& axis, const Degree angle);
  void rotate(const Vector3& axis, const Radians angle);
  void rotate(const Quaternion& q);

  // walk along with z axis
  // 很多设计当中, 提供了两种模式：
  // 一种是陆地模式，这种模式下，只能在陆地上按照 camera_z 在地平面的分量上行走
  // 另外一种是飞行模式，这种模式下，可以直接按照 camera_z 飞行
  //
  void fly(float step);
  void walk(float step);
  void strafe(float step);

  Vector3 right() const;
  Vector3 up() const;
  Vector3 direction() const;
  const Quaternion& orientation() const { return orientation_;}

  const Vector3& position() const { return position_;}
  void SetPosition(const Vector3& pos) { position_ = pos;}
  void SetPosition(float x, float y, float z) { position_ = Vector3(x, y, z);}
  Matrix4 world() const;
 protected:
  Vector3 position_;
  Quaternion orientation_;
};

inline MovableObject::MovableObject()
    : position_(0.0f, 0.0f, 0.0f)
    , orientation_(Vector3(0.0, 1.0, 0.0), Degree(0.0f)) {
}

inline MovableObject::MovableObject(const Vector3& position)
    : position_(position)
    , orientation_(Vector3(0.0, 1.0, 0.0), Degree(0.0f)) {
}

inline MovableObject::MovableObject(const Vector3& pos, const Quaternion& orient)
    : position_(pos)
    , orientation_(orient) {
}

inline MovableObject::MovableObject(const MovableObject& obj)
    : position_(obj.position())
    , orientation_(obj.orientation_) {
}

inline MovableObject& MovableObject::operator = (const MovableObject& obj) {
  position_ = obj.position();
  orientation_ = obj.orientation_;
  return *this;
}

inline void MovableObject::pitch(const Degree angle) {
  pitch(Radians(angle));
}

inline void MovableObject::yaw(const Degree angle) {
  yaw(Radians(angle));
}

inline void MovableObject::roll(const Degree angle) {
  roll(Radians(angle));
}

inline void MovableObject::pitch(const Radians angle) {
  Vector3 axis = orientation_ * Vector3::kUnitX;
  rotate(axis, angle);
}

inline void MovableObject::yaw(const Radians angle) {
  Vector3 axis = orientation_ * Vector3::kUnitY;
  rotate(axis, angle);
}

inline void MovableObject::roll(const Radians angle) {
  Vector3 axis = orientation_ * Vector3::kUnitZ;
  rotate(axis, angle);
}

inline void MovableObject::rotate(const Vector3& axis, const Radians rad) {
  Quaternion q(axis, rad);
  rotate(q);
}

inline void MovableObject::rotate(const Vector3& axis, const Degree angle) {
  Quaternion q(axis, angle);
  rotate(q);
}

inline void MovableObject::rotate(const Quaternion& q) {
  DCHECK_FLOAT_EQ(q.length(), 1.0f);
  orientation_ = q * orientation_;
  orientation_.Normalize();
}

inline void MovableObject::walk(float step) {
  position_ +=  direction() * step;
}

inline void MovableObject::fly(float step) {
  position_ += up() * step;
}

inline void MovableObject::strafe(float step) {
  position_ += right() * step;
}

inline Vector3 MovableObject::right() const {
  return orientation_ * Vector3::kUnitX;
}

inline Vector3 MovableObject::up() const {
  return orientation_ * Vector3::kUnitY;
}

inline Vector3 MovableObject::direction() const {
  return orientation_ * (-Vector3::kUnitZ);
}

inline azer::Matrix4 MovableObject::world() const {
  return std::move(Translate(position_)) *
      std::move(orientation_.ToMatrix());
}
}  // namespace azer

