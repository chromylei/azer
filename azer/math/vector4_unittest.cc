#include "azer/math/math.h"
#include "gtest/gtest.h"

using azer::Matrix4;
using azer::Vector4;


TEST(Vector4, Access) {
}

TEST(Vector4, MultiplyWithScalar) {
  
}

TEST(Vector4, MultiplyWithVector) {
  Vector4 v1(1.0f, 2.0f, 3.0f, 4.0f);
  Vector4 v2 = v1 * 3;
  ASSERT_EQ(v1, Vector4(1.0f, 2.0f, 3.0f, 4.0f));
  ASSERT_EQ(v2.x, 3.0f);
  ASSERT_EQ(v2.y, 6.0f);
  ASSERT_EQ(v2.z, 9.0f);
  ASSERT_EQ(v2.w, 12.0f);
}

TEST(Vector4, MultiplyWithMatrix1) {
  const int kTestCaseNum = 2;
  Matrix4 mat[] = {
    Matrix4(1.0, 0.0, 0.0, 1.0,
            0.0, 1.0, 0.0, 2.0,
            0.0, 0.0, 1.0, 3.0,
            0.0, 0.0, 0.0, 1.0),
    Matrix4(1.0,  5.0,  9.0, 13.0,
            2.0,  6.0, 10.0, 14.0,
            3.0,  7.0, 11.0, 15.0,
            4.0,  8.0, 12.0, 16.0),
  };

  Vector4 vec[] = {
    Vector4(1.0f, 1.0f, 1.0f, 1.0f),
    Vector4(1.0f, 2.0f, 3.0f, 4.0f),
  };
  
  Vector4 expect[] = {
    Vector4(1, 1, 1, 7),
    Vector4(30, 70, 110, 150)
  };

  for (int i = 0; i < kTestCaseNum; ++i) {
    Vector4 result =  vec[i] * mat[i];
    ASSERT_EQ(result, expect[i]);
  }
}
