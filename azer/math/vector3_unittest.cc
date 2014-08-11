#include "azer/math/math.h"
#include "gtest/gtest.h"

using azer::Vector3;

TEST(Vector3, MultiplyWithVector) {
  Vector3 v1(1.0f, 2.0f, 3.0f);
  Vector3 v2 = v1 * 3;
  ASSERT_EQ(v2.x, 3.0f);
  ASSERT_EQ(v2.y, 6.0f);
  ASSERT_EQ(v2.z, 9.0f);
}

TEST(Vector3, CrossProduct) {
  {
    Vector3 v1(2, 3, 4);
    Vector3 v2(5, 6, 7);
    Vector3 v3 = azer::CrossProduct(v1, v2);
    const Vector3 expect(-3, 6, -3);
    ASSERT_EQ(expect, v3);
  }
}
