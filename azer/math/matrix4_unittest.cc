#include "azer/math/math.h"
#include "gtest/gtest.h"

using azer::Matrix4;
using azer::Vector4;

TEST(Matrix4, Base) {
}

TEST(Matrix4, AccessByIndex) {
}

TEST(Matrix4, MultiplyWithVector) {
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
    Vector4(2, 3, 4, 1),
    Vector4(90, 100, 110, 120)
  };

  for (int i = 0; i < kTestCaseNum; ++i) {
    Vector4 result = mat[i] * vec[i];
    ASSERT_EQ(expect[i], result);
  }
}

TEST(Matrix4, MatrixProduction) {
  Matrix4 mat1[] = {
    Matrix4(1.0, 0.0, 0.0, 1.0,
            0.0, 1.0, 0.0, 2.0,
            0.0, 0.0, 1.0, 3.0,
            0.0, 0.0, 0.0, 1.0),
    Matrix4(31, 12,  8, 27,
            45, 61, 22, 37,
            93, 30, 90, 95,
            28, 15, 11, 32),
  };

  Matrix4 mat2[] = {
    Matrix4(1, 5, 8, 4,
            2, 6, 7, 3,
            3, 7, 6, 2,
            4, 8, 5, 1),
    Matrix4(81, 11,  1,   7,
            49, 13,  31, 37,
            63, 17,  37, 21,
            72, 19,  83, 13),
  };
  
  Matrix4 expect[][2] = {
    {
      Matrix4(5,  13, 13, 5,
              10, 22, 17, 5,
              15, 31, 21, 5,
              4,   8,  5, 1),
      Matrix4(1, 5, 8, 39,
              2, 6, 7, 38,
              3, 7, 6, 37,
              4, 8, 5, 36),
    },

    {
      Matrix4(5547, 1146, 2940, 1180,
              10684, 2365, 5821, 3515,
              21513, 4748, 12238, 4886,
              6000, 1298, 3556, 1398),
      Matrix4(3295, 1778, 1057, 2913,
              6023, 2866, 3875, 5933,
              6747, 3218, 4439, 6517,
              11170, 4708, 8607, 10948),
    }
  };

  for (int i = 0; i < 1; ++i) {
    const Matrix4& A = mat1[i];
    const Matrix4& B = mat2[i];
    Matrix4 r1 = A * B;
    Matrix4 r2 = B * A;
    ASSERT_EQ(expect[i][0], r1);
    ASSERT_EQ(expect[i][1], r2);
  }
}
