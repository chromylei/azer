#pragma once

#include "gtest/gtest.h"

#define VECTOR3_ASSERT_EQ(v1, v2)               \
  ASSERT_FLOAT_EQ(v1.x(), v2.x());              \
  ASSERT_FLOAT_EQ(v1.y(), v2.y());              \
  ASSERT_FLOAT_EQ(v1.z(), v2.z())
