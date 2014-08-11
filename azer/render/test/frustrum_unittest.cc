#include "azer/math/math.h"
#include "azer/render/frustrum.h"
#include "azer/render/camera.h"
#include "azer/render/axis_aligned_box.h"
#include "gtest/gtest.h"

using azer::Camera;
using azer::Vector3;
using azer::Vector4;
using azer::Matrix4;
using azer::AxisAlignedBox;

TEST(FrustrumTest, Point) {
  struct PointTestCase {
    Vector4 point;
    bool expect;

    PointTestCase(const Vector4& pt, bool e)
        : point(pt), expect(e) {}
  };

  PointTestCase cases[] = {
    PointTestCase(Vector4(-0.5, -0.5, 2.5, 1.0), false),
    PointTestCase(Vector4(0.5, 0.5, 3.5, 1.0), false),
    PointTestCase(Vector4(-0.5, -0.5, -3.5, 1.0), true),
    PointTestCase(Vector4(0.5, 0.5,   -2.5, 1.0), true),
  };
  Camera camera(Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0));
  for (size_t i = 0; i < arraysize(cases); ++i) {
    EXPECT_EQ(camera.frustrum().IsVisible(cases[i].point), cases[i].expect) << i;
  }
}

TEST(FrustrumTest, AABB) {
  Camera camera(Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0));
  AxisAlignedBox front_aabb(Vector4(-0.5, -0.5, -1.5, 1.0),
                           Vector4(0.5, 0.5, 1.5, 1.0));
  
  EXPECT_TRUE(front_aabb.IsVisible(camera.frustrum(), Matrix4::kIdentity));

  AxisAlignedBox back_aabb(Vector4(-0.5, -0.5, 0.5, 1.0),
                           Vector4(0.5, 0.5, 1.5, 1.0));
  
  EXPECT_FALSE(back_aabb.IsVisible(camera.frustrum(), Matrix4::kIdentity));
}
