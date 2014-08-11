#pragma once

#include <ostream>
#include <utility>

#include "azer/math/consts.h"

namespace azer {

class Degree;
class Radians;

class Degree {
 public:
  Degree() : value_(0.0f) {}
  explicit Degree(const Radians r);
  Degree(const float v) : value_(v) {}
  Degree(const Degree& d) : value_(d.value_) {}

  float value() const { return value_;}
  friend Radians operator / (Radians rad1, float scalar);

  Degree& operator * (float val);
  Degree& operator * (double val);
 private:
  float value_;
  friend class Radians;
};

class Radians {
 public:
  Radians() : value_(0.0f) {}
  explicit Radians(const Degree degree) {
    value_ = degree.value_ * kPI / 180.0f;
  }

  Radians(const Radians& radian) {
    value_ = radian.value_;
  }

  Radians(const float r) : value_(r) {}

  Radians& operator * (float val);
  Radians& operator * (double val);
  operator float() const{ return value_; }

  friend Radians operator + (Radians r1, Radians r2);
  friend Radians operator - (Radians r1, Radians r2);
  friend Radians operator / (Radians rad1, float scalar);

  float value() const { return value_;}

  friend std::ostream& operator << (std::ostream& os, Radians radians);
 private:
  float value_;
  friend class Degree;
};

inline Degree::Degree(const Radians r) {
  value_ = r.value_ * 180.0f / kPI;
}

inline Degree& Degree::operator * (float val) {
  value_ *= val;
  return *this;
}

inline Degree& Degree::operator * (double val) {
  value_ *= (float)val;
  return *this;
}

inline Degree operator / (Degree degree, float scalar) {
  return Degree(degree.value() / scalar);
}

inline Radians& Radians::operator * (float val) {
  value_ *= val;
  return *this;
}

inline Radians& Radians::operator * (double val) {
  value_ *= (float)val;
  return *this;
}

inline Radians operator / (Radians rad1, float scalar) {
  return Radians(rad1.value_ / scalar);
}

inline Radians operator + (Radians r1, Radians r2) {
  return Radians(r1.value_ + r2.value_);
}

inline Radians operator - (Radians r1, Radians r2) {
  return Radians(r1.value_ - r2.value_);
}

inline float sin(const Radians r) { return std::sin(r.value()); }
inline float sin(const Degree degree) { return sin(Radians(degree)); }
inline float cos(const Radians r) { return std::cos(r.value());}
inline float cos(const Degree degree) { return cos(Radians(degree));}
inline float tan(const Radians r) { return std::tan(r.value());}
inline float tan(const Degree degree) { return tan(Radians(degree));}

inline std::ostream& operator << (std::ostream& os, Radians radians) {
  os << "Radians(PI * " << radians.value_ / kPI << ")";
  return os;
}
}  // namespace azer
