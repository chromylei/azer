#pragma once

namespace azer {
class Ray {
 public:
  Ray() {}
  Ray(const Vector3& pos, const Vector3& dir)
      : position_(pos)
      , directional_(dir) {
  }
  
  Ray(const Ray& ray) {
    *this = ray;
  }

  Ray& operator = (const Ray& ray) {
    this->position_ = ray.position_;
    this->directional_ = ray.directional_;
    return *this;
  }

  const Vector3& position() const { return position_;}
  const Vector3& directional() const { return directional_;}
 private:
  Vector3 position_;
  Vector3 directional_;
};
}  // namespace azer
