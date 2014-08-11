#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "azer/math/math.h"
#include <ostream>

inline std::ostream& operator << (std::ostream& os, glm::mat4& mat) {
  os  << "mat4:"
      << "[" << "\n"
      << mat[0][0] << ",\t" << mat[1][0] << ",\t" << mat[2][0] << ",\t" << mat[3][0]
      << "\n"
      << mat[0][1] << ",\t" << mat[1][1] << ",\t" << mat[2][1] << ",\t" << mat[3][1]
      << "\n"
      << mat[0][2] << ",\t" << mat[1][2] << ",\t" << mat[2][2] << ",\t" << mat[3][2]
      << "\n"
      << mat[0][3] << ",\t" << mat[1][3] << ",\t" << mat[2][3] << ",\t" << mat[3][3]
      << "\n"
      << "]";
  return os;
}

inline std::ostream& operator << (std::ostream& os, glm::vec3& v) {
  os << "glm::vec3[" << v.x << ",\t" << v.y << ",\t" << v.z << "]";
  return os;
}

inline std::ostream& operator << (std::ostream& os, glm::vec4& v) {
  os << "glm::vec4[" << v.x << ",\t" << v.y << ",\t" << v.z
     << ",\t" << v.w << "]";
  return os;
}

inline std::ostream& operator << (std::ostream& os, glm::quat& v) {
  os << "glm::vec4[" << v.w << ",\t" << v.x << ",\t" << v.y << ",\t" << v.z << "]";
  return os;
}

inline bool operator == (const glm::vec4& v1, const azer::Vector4& v2) {
  return (std::abs(v1.x - v2.x) < 0.00004)
      && (std::abs(v1.y - v2.y) < 0.00004)
      && (std::abs(v1.z - v2.z) < 0.00004)
      && (std::abs(v1.w - v2.w) < 0.00004);
}

inline bool operator == (const glm::vec3& v1, const azer::Vector3& v2) {
  return (std::abs(v1.x - v2.x) < 0.00004)
      && (std::abs(v1.y - v2.y) < 0.00004)
      && (std::abs(v1.z - v2.z) < 0.00004);
}

inline bool operator == (const glm::mat4& mat1, const azer::Matrix4& mat2) {
  return
      (std::abs(mat1[0][0] - mat2[0][0]) < 0.0001)
      && (std::abs(mat1[0][1] - mat2[1][0]) < 0.0001)
      && (std::abs(mat1[0][2] - mat2[2][0]) < 0.0001)
      && (std::abs(mat1[0][3] - mat2[3][0]) < 0.0001)
      && (std::abs(mat1[1][0] - mat2[0][1]) < 0.0001)
      && (std::abs(mat1[1][1] - mat2[1][1]) < 0.0001)
      && (std::abs(mat1[1][2] - mat2[2][1]) < 0.0001)
      && (std::abs(mat1[1][3] - mat2[3][1]) < 0.0001)
      && (std::abs(mat1[2][0] - mat2[0][2]) < 0.0001)
      && (std::abs(mat1[2][1] - mat2[1][2]) < 0.0001)
      && (std::abs(mat1[2][2] - mat2[2][2]) < 0.0001)
      && (std::abs(mat1[2][3] - mat2[3][2]) < 0.0001)
      && (std::abs(mat1[3][0] - mat2[0][3]) < 0.0001)
      && (std::abs(mat1[3][1] - mat2[1][3]) < 0.0001)
      && (std::abs(mat1[3][2] - mat2[2][3]) < 0.0001)
      && (std::abs(mat1[3][3] - mat2[3][3]) < 0.0001);
}

inline bool operator == (const azer::Matrix4& mat2, const glm::mat4& mat1) {
  return mat1 == mat2;
}

inline glm::vec3 glm_conv(const azer::Vector3& v) {
  return glm::vec3(v.x, v.y, v.z);
}

inline glm::vec4 glm_conv(const azer::Vector4& v) {
  return glm::vec4(v.x, v.y, v.z, v.w);
}
