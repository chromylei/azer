
namespace azer {

inline Vector4::Vector4() {
  this->x = 0.0f;
  this->y = 0.0f;
  this->z = 0.0f;
  this->w = 0.0f;
}

inline Vector4::Vector4(const Vector3& vec, float w) {
  this->x = vec.x;
  this->y = vec.y;
  this->z = vec.z;
  this->w = w;
}

inline Vector4::Vector4(float x, float  y, float z, float w) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->w = w;
}

inline Vector4::Vector4(const Vector4& vec) {
  this->x = vec.x;
  this->y = vec.y;
  this->z = vec.z;
  this->w = vec.w;
}

inline float& Vector4::operator[](int index) {
  DCHECK(index >= 0 && index < 4);
  return ((float*)this)[index];
}

inline const float& Vector4::operator[](int index) const {
  DCHECK(index >= 0 && index < 4);
  return ((float*)this)[index];
}

inline float Vector4::dot(const Vector4& vec) const {
  return vec.x * x + vec.y * y + vec.z * z;
}

inline float Vector4::length() const {
  return std::sqrt(dot(*this));
}

inline bool operator == (const Vector4& v1, const Vector4& v2) {
  return Float(v1.x).AlmostEquals(Float(v2.x))
      && Float(v1.y).AlmostEquals(Float(v2.y))
      && Float(v1.z).AlmostEquals(Float(v2.z))
      && Float(v1.w).AlmostEquals(Float(v2.w));
}

inline Vector4 Vector4::operator - () const {
  return Vector4(-x, -y, -z, -w);
}

inline Vector4& Vector4::operator += (const Vector4& vec) {
  this->x += vec.x;
  this->y += vec.y;
  this->z += vec.z;
  this->w += vec.z;
  return *this;
}

inline Vector4& Vector4::operator -= (const Vector4& vec) {
  this->x -= vec.x;
  this->y -= vec.y;
  this->z -= vec.z;
  this->w -= vec.w;
  return *this;
}

inline Vector4& Vector4::operator += (const float scalar) {
  this->x += scalar;
  this->y += scalar;
  this->z += scalar;
  this->w += scalar;
  return *this;
}

inline Vector4& Vector4::operator -= (const float scalar) {
  this->x -= scalar;
  this->y -= scalar;
  this->z -= scalar;
  this->w -= scalar;
  return *this;
}

inline Vector4& Vector4::operator *= (const float scalar) {
  this->x *= scalar;
  this->y *= scalar;
  this->z *= scalar;
  this->w *= scalar;
  return *this;
}

inline Vector4& Vector4::operator /= (const float scalar) {
  this->x /= scalar;
  this->y /= scalar;
  this->z /= scalar;
  this->w /= scalar;
  return *this;
}

inline Vector4& Vector4::Normalize() {
  float len = length();
  // Will also work for zero-sized vectors, but will change nothing
  // We're not using epsilons because we don't need to.
  // Read http://www.ogre3d.org/forums/viewtopic.php?f=4&t=61259
  if (len > 0) {
    float invlen = 1 / len;
    *this *= invlen;
  }

  return *this;
}

inline Vector4 Vector4::NormalizeCopy() const {
  Vector4 vec(*this);
  return vec.Normalize();
}

inline Vector4 operator * (const float v[4], const Matrix4& mat) {
  return Vector4(
      v[0] * mat[0][0] + v[1] * mat[1][0] + v[2] * mat[2][0] + v[3] * mat[3][0],
      v[0] * mat[0][1] + v[1] * mat[1][1] + v[2] * mat[2][1] + v[3] * mat[3][1],
      v[0] * mat[0][2] + v[1] * mat[1][2] + v[2] * mat[2][2] + v[3] * mat[3][2],
      v[0] * mat[0][3] + v[1] * mat[1][3] + v[2] * mat[2][3] + v[3] * mat[3][3]
                 );
}

inline Vector4 operator * (const Vector4& vec, const Matrix4& mat) {
  return  vec.Pointer() * mat;
}

inline Vector4 operator *(const Vector4& v, const float scalar) {
  return Vector4(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
}

inline Vector4 operator /(const Vector4& v, const float scalar) {
  return Vector4(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar);
}

inline Vector4 operator + (const Vector4& v1, const Vector4& v2) {
  return Vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

inline Vector4 operator - (const Vector4& v1, const Vector4& v2) {
  return Vector4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z,
                 v1.w - v2.w);
}

inline std::ostream& operator << (std::ostream& os, const Vector4& v) {
  os << "vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
  return os;
}

}  // namespace azer
