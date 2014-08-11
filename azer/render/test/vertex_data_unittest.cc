#include "azer/render/vertex_data.h"
#include "azer/math/math.h"
#include "base/basictypes.h"
#include "base/logging.h"
#include "gtest/gtest.h"


using namespace azer;
TEST(VertexData, Desc1) {
  const VertexDesc::Desc kVertexDescs[] = {
    {"POSITION", 0, azer::kVec4},
  };
  VertexDescPtr desc(new VertexDesc(kVertexDescs, arraysize(kVertexDescs)));
  ASSERT_EQ(desc->offset(0), 0);
  ASSERT_EQ(desc->element_num(), 1);
  ASSERT_EQ(desc->stride(), 4 * sizeof(float));
}

TEST(VertexData, Desc2) {
  const VertexDesc::Desc kVertexDescs[] = {
    {"POSITION", 0, azer::kVec4},
    {"COLOR", 0, azer::kVec4},
  };
  VertexDescPtr desc(new VertexDesc(kVertexDescs, arraysize(kVertexDescs)));
  ASSERT_EQ(desc->offset(0), 0);
  ASSERT_EQ(desc->offset(1), sizeof(azer::Vector4));
  ASSERT_EQ(desc->element_num(), 2);
  ASSERT_EQ(desc->stride(), 8 * sizeof(float));
}

TEST(VertexData, Desc3) {
  const VertexDesc::Desc kVertexDescs[] = {
    {"POSITION", 0, azer::kVec4},
    {"COLOR", 0, azer::kVec4},
    {"TEXCOORD", 0, azer::kVec2},
  };
  VertexDescPtr desc(new VertexDesc(kVertexDescs, arraysize(kVertexDescs)));
  ASSERT_EQ(desc->offset(0), 0);
  ASSERT_EQ(desc->offset(1), sizeof(azer::Vector4));
  ASSERT_EQ(desc->offset(2), 2 * sizeof(azer::Vector4));
  ASSERT_EQ(desc->element_num(), 3);
  ASSERT_EQ(desc->stride(), 10 * sizeof(float));
}

TEST(VertexData, Desc4) {
  const VertexDesc::Desc kVertexDescs[] = {
    {"POSITION", 0, azer::kVec4},
    {"COLOR", 0, azer::kVec3},
    {"TEXCOORD", 0, azer::kVec2},
  };
  VertexDescPtr desc(new VertexDesc(kVertexDescs, arraysize(kVertexDescs)));
  ASSERT_EQ(desc->offset(0), 0);
  ASSERT_EQ(desc->offset(1), sizeof(azer::Vector4));
  ASSERT_EQ(desc->offset(2), desc->offset(1) +  sizeof(azer::Vector3));
  ASSERT_EQ(desc->element_num(), 3);
  ASSERT_EQ(desc->stride(), 9 * sizeof(float));
}
