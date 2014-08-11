#include "azer/math/math.h"
#include "gtest/gtest.h"
#include <cmath>

using azer::Vector3;
using azer::Degree;
using azer::Radians;

TEST(Degree, sin) {
  ASSERT_EQ(azer::sin(Degree(90.0f)), 1);
  ASSERT_EQ(azer::sin(Degree(0.0)), 0.0);
}

TEST(Degree, cosine) {
  ASSERT_LE(std::abs(azer::cos(Degree(90.0f)) - 0), 0.00001);
  ASSERT_EQ(cos(Degree(0.0)), 1.0f);
}

TEST(Degree, convert) {
  Degree d(Radians(3.1415927f));
  ASSERT_LT(std::abs(d.value() - 180.0f), 0.0001);
}

TEST(Radians, sin) {
  ASSERT_EQ(sin(Radians(azer::kPI / 2.0f)), 1);
  ASSERT_EQ(sin(Radians(0.0)), 0.0);
}

TEST(Radians, cosine) {
  ASSERT_LE(std::abs(cos(Radians(azer::kPI / 2.0f)) - 0), 0.00001);
  ASSERT_EQ(cos(Radians(0.0)), 1.0);
}

TEST(Radians, Convert) {
  Radians r(Degree(90.0f));
  ASSERT_EQ(r.value(), azer::kPI / 2.0f);
}
