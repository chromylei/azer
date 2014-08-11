#include "gtest/gtest.h"
#include "azer/afx/compiler/astdump.h"
#include "azer/afx/compiler/parser.h"
#include "azer/afx/compiler/testutil.h"
#include "azer/afx/compiler/type.h"
#include "azer/afx/compiler/value.h"

using azer::afx::Value;
TEST(Value, Plus) {
  // int32 + int32
  {
    Value v1(10), v2(10);
    Value ret = v1 + v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetInt32(), 20);
  }
  // uint32 + int32
  {
    Value v1(10u), v2(10);
    Value ret = v1 + v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetInt32(), 20);
  }

  // int32 + float
  {
    Value v1(1), v2(2.0f);
    Value ret = v1 + v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetFloat(), 3.0f);
  }

  // float + float
  {
    Value v1(0.5f), v2(1.0f);
    Value ret = v1 + v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.type()->type(), azer::afx::kFloat);
    ASSERT_EQ(ret.GetFloat(), 1.5f);
  }
}


TEST(Value, Substract) {
  // int32 + int32
  {
    Value v1(10), v2(1);
    Value ret = v1 - v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetInt32(), 9);
  }
  // uint32 + int32
  {
    Value v1(10u), v2(8);
    Value ret = v1 - v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetInt32(), 2);
  }

  // int32 + float
  {
    Value v1(1), v2(2.0f);
    Value ret = v1 - v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetFloat(), -1.0f);
  }

  // float + float
  {
    Value v1(0.5f), v2(1.0f);
    Value ret = v1 - v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.type()->type(), azer::afx::kFloat);
    ASSERT_EQ(ret.GetFloat(), -0.5f);
  }
}

TEST(Value, Mul) {
  // int32 + int32
  {
    Value v1(10), v2(1);
    Value ret = v1 * v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetInt32(), 10);
  }
  // uint32 + int32
  {
    Value v1(10u), v2(8);
    Value ret = v1 * v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetInt32(), 80);
  }

  // int32 + float
  {
    Value v1(1), v2(2.0f);
    Value ret = v1 * v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetFloat(), 2.0f);
  }

  // float + float
  {
    Value v1(0.5f), v2(1.0f);
    Value ret = v1 * v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.type()->type(), azer::afx::kFloat);
    ASSERT_EQ(ret.GetFloat(), 0.5f);
  }

  // vector * vector
  {
    using namespace azer::afx;
    Value v1(kVector2), v2(kVector2);
    Value ret = v1 * v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_EQ(ret.type()->type(), kFloat);
  }
  {
    using namespace azer::afx;
    Value v1(kVector3), v2(kVector3);
    Value ret = v1 * v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_EQ(ret.type()->type(), kFloat);
  }
  {
    using namespace azer::afx;
    Value v1(kVector4), v2(kVector4);
    Value ret = v1 * v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_EQ(ret.type()->type(), kFloat);
  }

  // matrix * vector

  // matrix * matrix
}

TEST(Value, MulInvalid) {
  using namespace azer::afx;
  {
    Value v1(kVector3), v2(kVector2);
    Value ret = v1 * v2;
    ASSERT_FALSE(ret.valid());
  }
  {
    Value v1(kVector2), v2(kVector3);
    Value ret = v1 * v2;
    ASSERT_FALSE(ret.valid());
  }
  {
    Value v1(kVector2), v2(kVector4);
    Value ret = v1 * v2;
    ASSERT_FALSE(ret.valid());
  }
  {
    Value v1(kVector4), v2(kVector3);
    Value ret = v1 * v2;
    ASSERT_FALSE(ret.valid());
  }
}

TEST(Value, Div) {
  // int32 / int32
  {
    Value v1(10), v2(1);
    Value ret = v1 / v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetInt32(), 10);
  }
  // uint32 / int32
  {
    Value v1(10u), v2(8);
    Value ret = v1 / v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetInt32(), 1);
  }

  // int32 / float
  {
    Value v1(1), v2(2.0f);
    Value ret = v1 / v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.GetFloat(), 0.5f);
  }

  // float / float
  {
    Value v1(0.5f), v2(1.0f);
    Value ret = v1 / v2;
    ASSERT_TRUE(ret.valid());
    ASSERT_TRUE(ret.assigned());
    ASSERT_EQ(ret.type()->type(), azer::afx::kFloat);
    ASSERT_EQ(ret.GetFloat(), 0.5f);
  }
}

TEST(Value, DivInvalid) {
  // int32 / int32
  {
    Value v1(true), v2(false);
    Value ret = v1 / v2;
    ASSERT_TRUE(!ret.valid());
    ASSERT_TRUE(!ret.assigned());
  }
}

TEST(Value, Assign) {
  // int32 / int32
  {
    Value v1(true), v2(false);
    Value ret = v1 / v2;
    ASSERT_TRUE(!ret.valid());
    ASSERT_TRUE(!ret.assigned());
  }
}

TEST(Value, LogicalOperation) {
  {
    Value v1(1), v2(2); Value r = v1 > v2;
    ASSERT_TRUE(r.valid());
    ASSERT_TRUE(r.assigned());
    ASSERT_EQ(r.type()->type(), azer::afx::kBoolean);
    ASSERT_EQ(r.GetBoolean(), false);
  }

  {
    Value v1(1), v2(2.0f); Value r = v1 > v2;
    ASSERT_TRUE(r.valid());
    ASSERT_TRUE(r.assigned());
    ASSERT_EQ(r.type()->type(), azer::afx::kBoolean);
    ASSERT_EQ(r.GetBoolean(), false);
  }
}

TEST(Value, LogicalOperationWithoutAssign) {
  {
    int arr[] = {1, 2, 3};
    Value v1(1), v2(arr, (int)arraysize(arr));
    Value r = v1 > v2;
    ASSERT_FALSE(r.valid());
    ASSERT_FALSE(r.assigned());
  }
}

TEST(Value, LogicalOperationInvalid) {
  {
    Value v1(1), v2(1.0f);
    Value r = v1 && v2;
    ASSERT_FALSE(r.valid());
    ASSERT_FALSE(r.assigned());
  }

  {
    Value v1(1), v2(1.0f);
    Value r = v1 || v2;
    ASSERT_FALSE(r.valid());
    ASSERT_FALSE(r.assigned());
  }
}
