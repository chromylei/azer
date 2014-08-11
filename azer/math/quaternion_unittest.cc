#include "azer/math/math.h"
#include "azer/math/test_util.h"
#include "azer/math/glm_util.h"
#include "gtest/gtest.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"


// 编写单元测试时可以使用 matlab 帮助产生测试用例
// matlab 当中 quaternion 常用的函数包括
// 1. 根据 Eular 生成 Quaternioin: angle2quat(0.0, 3.141593/2.0, 0.0, 'XYZ')
// 2. Quaternion 相称: quatmultiply
// 3. Quaternion 旋转: quatrotate(quat, vec)
// 4.


using azer::kPI;
using azer::Vector3;
using azer::Vector4;
using azer::Matrix4;
using azer::Degree;
using azer::Radians;
using azer::Quaternion;

TEST(Quaternion, base) {
  /*
  Vector3 vec(1.0, 0, 0);
  Quaternion r(Vector3(0.0, 1.0, 0.0), Radians(kPI / 2.0));
  Quaternion q(0.0, 1.0, 0.0, 0.0);
  std::cout << r << std::endl;
  Quaternion t = q * r.UnitInverse();
  std::cout << t << std::endl;
  Quaternion f = r * t;
  std::cout << f << std::endl;
  */
}

TEST(Quaternion, product) {
  const int kTestCase = 3;
  Quaternion q1[] = {
    Quaternion(1, 0, 1, 0),
    Quaternion(0, 0.7071f, 0.0f, -0.7071f),
    Quaternion(0.7071f, 0.0f, 0.7071f, 0.0f),
  };
  Quaternion q2[] = {
    Quaternion(1.0f, 0.5f, 0.5f, 0.75f),
    Quaternion(0.7071f, 0.0f, 0.7071f, 0.0f),
    Quaternion(0.0f, 0.7071f, 0.0f, -0.7071f),
  };
  Quaternion exp[] = {
    Quaternion(0.5f, 1.25f, 1.5f, 0.25f),
    Quaternion(0.0f, 1.0f, 0.0f, 0.0f),  // 实际上为 (0， 1， 0， 0)
    Quaternion(0.0f, 0.0f, 0.0f, -1.0),
  };
  

  for (int i = 0; i < kTestCase; ++i) {
    Quaternion r = q1[i] * q2[i];
    ASSERT_EQ(r, exp[i]);
  }
}

TEST(Quaternion, UnitInverse) {
  const int kTestCase = 1;
  Quaternion q[] = {Quaternion(1, 0, 1, 0).NormalizeCopy()};
  for (int i = 0; i < kTestCase; ++i) {
    ASSERT_EQ(q[i] * q[i].UnitInverse(), Quaternion::kIdentity);
  }
}

TEST(Quaternion, FromRotateMatrix) {
  const int kTestCase = 1;
  Matrix4 mat[] = {
    azer::RotateY(Radians(kPI / 2.0f)),
  };

  Quaternion exp[] = {
    Quaternion(0.70710670f, 0.0f, -0.70710670f, 0.0f),
  };

  for (int i = 0; i < kTestCase; ++i) {
    ASSERT_EQ(Quaternion(mat[i]), exp[i]);
  }
}

TEST(Quaternion, FromRotate) {
  const int kTestCase = 1;
  Quaternion q[] = {
    Quaternion(Vector3(0.0f, 1.0f, 0.0f), Degree(90.0f)),
  };

  Quaternion exp[] = {
    Quaternion(0.70710671f, 0.0f, 0.70710683f, 0.0f),
  };

  for (int i = 0; i < kTestCase; ++i) {
    ASSERT_EQ(q[i], exp[i]);
  }
}

TEST(Quaternion, Rotation) {
  const int kTestCase = 1;
  Quaternion rq[] = {
    Quaternion(Vector3(0.0f, 1.0f, 0.0f), Radians(kPI / 2.0f)),
  };

  Vector3 vec[] = {
    Vector3(1.0f, 0.0f, 0.0f),
  };

  Vector3 exp[] = {
    Vector3(0.0f, 0.0f, -1.0f),
  };

  for (int i = 0; i < kTestCase; ++i) {
    Vector3 r = rq[i] * vec[i];
    ASSERT_EQ(r, exp[i]);
  }
}

TEST(Quaternion, RotationSequence) {
  using namespace glm;
  const Vector3 vec(0.0, 0.0f, 1.0f);
  Vector3 r1, r2, r3, r4;
  {
    const Vector3 axis(0.0, 1.0, 0.0);
    Quaternion q(axis, Degree(15.0f));
    r1 = q * vec;
    glm::quat gmqt = glm::angleAxis(15.0f, glm_conv(axis));
    glm::vec3 v = gmqt * glm_conv(vec);
    ASSERT_TRUE(v == r1);
  }

  {
    const Vector3 axis(1.0, 0.0, 0.0);
    Quaternion q(axis, Degree(15.0f));
    r2 = q * r1;
    glm::quat gmqt = glm::angleAxis(15.0f, glm_conv(axis));
    glm::vec3 v = gmqt * glm_conv(r1);
    ASSERT_TRUE(v == r2);
  }

  {
    const Vector3 axis(0.0, 1.0, 0.0);
    Quaternion q(axis, Degree(-15.0f));
    r3 = q * r2;
    glm::quat gmqt = glm::angleAxis(-15.0f, glm_conv(axis));
    glm::vec3 v = gmqt * glm_conv(r2);
    ASSERT_TRUE(v == r3);
  }

  {
    const Vector3 axis(1.0, 0.0, 0.0);
    Quaternion q(axis, Degree(-15.0f));
    r4 = q * r3;
    glm::quat gmqt = glm::angleAxis(-15.0f, glm_conv(axis));
    glm::vec3 v = gmqt * glm_conv(r3);
    ASSERT_TRUE(v == r4);
  }
}

TEST(Quaternion, UnitRotateAxis) {
  const int kTestCase = 3;
  Vector3 vec[] = {
    Vector3(1, 0, 0),
    Vector3(0, 1, 0),
    Vector3(0, 0, 1),
  };

  Vector3 axis[] = {
    Vector3(0.0f, 1.0f, 0.0f),
    Vector3(0.0f, 1.0f, 0.0f),
    Vector3(0.0f, 1.0f, 0.0f)
  };

  Vector3 exp[] = {Vector3(0.0f, 0.0f, -1.0f),
                   Vector3(0.0f, 1.0f, 0.0f),
                   Vector3(1.0f, 0.0f, 0.0f),
  };

  Degree degree[] = {
    Degree(90.0f), Degree(90.0f), Degree(90.0f),
  };
  
  for (int i = 0; i < kTestCase; ++i) {
    Vector3 ret = vec[i].RotateCopy(axis[i], degree[i]);
    ASSERT_EQ(ret, exp[i]);
  }
}

TEST(Quaternion, ToMatrix) {
  {
    const Vector3 axis(1.0, 0.0, 0.0);
    Quaternion q(axis, Degree(-15.0f));
    glm::quat glmquat = glm::angleAxis(-15.0f, glm_conv(axis));
    glm::mat4 glm_mat = glm::toMat4(glmquat);
    azer::Matrix4 mat = std::move(q.ToMatrix());
    ASSERT_TRUE(mat == glm_mat);
  }

  {
    const Vector3 axis(1.0, 0.70f, 0.70f);
    Quaternion q(axis, Degree(-15.0f));
    glm::quat glmquat = glm::angleAxis(-15.0f, glm_conv(axis));
    glm::mat4 glm_mat = glm::toMat4(glmquat);
    azer::Matrix4 mat = std::move(q.ToMatrix());
    ASSERT_TRUE(mat == glm_mat);
  }
}

TEST(Quaternion, Slerp) {
}
