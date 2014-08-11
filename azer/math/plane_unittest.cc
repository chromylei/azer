#include "azer/math/math.h"
#include "gtest/gtest.h"

using azer::vec3;
using azer::Ray;
using azer::TrianglePlane;

TEST(Plane, distance) {
  {
    // case1
    const vec3  plane_points[] = {
      vec3(1.0, 0.0f, 0.0f), vec3(0.0, 1.0f, 0.0f), vec3(0.0, 0.0f, 0.0f),
    };
  
    azer::Plane plane(plane_points);
    float dist = plane.distance(vec3(0.0f, 5.0f, 1.0f));
    EXPECT_TRUE(std::abs(dist - 1.0f) < 0.0001) << dist;
  }

  {
    // case2
    const vec3  plane_points[] = {
      vec3(1.0, 0.0f, 0.0f), vec3(0.0, 1.0f, 0.0f), vec3(0.0, 0.0f, 1.0f),
    };
  
    azer::Plane plane(plane_points);
    float dist = plane.distance(vec3(0.0f, 0.0f, 0.0f));
    EXPECT_TRUE(std::abs(dist - -0.57735f) < 0.0001) << dist;
  }

  {
    // case2
    const vec3  plane_points[] = {
      vec3(-1.0, 0.0f, 0.0f), vec3(0.0, -1.0f, 0.0f), vec3(0.0, 0.0f, -1.0f),
    };
  
    azer::Plane plane(plane_points);
    float dist = plane.distance(vec3(0.0f, 0.0f, 0.0f));
    EXPECT_TRUE(std::abs(dist - 0.57735f) < 0.0001) << dist;
  }
}


TEST(Plane, RayDistance) {
  {
    // case1
    const vec3  plane_points[] = {
      vec3(1.0, 0.0f, 0.0f), vec3(0.0, 1.0f, 0.0f), vec3(0.0, 0.0f, 1.0f),
    };

    vec3 pos(0.0f, 0.0f, 0.0f);
    vec3 dir(1.0f, 1.0f, 1.0f);
    dir.Normalize();
    azer::Plane plane(plane_points);
    Ray ray(pos, dir);
    float dist = plane.distance(ray);
    EXPECT_TRUE(std::abs(dist - 0.57735f) < 0.0001) << dist;
    EXPECT_TRUE(plane.belong(pos + dir * dist));
  }

  {
    // case2
    const vec3  plane_points[] = {
      vec3(-1.0, 0.0f, 0.0f), vec3(0.0, -1.0f, 0.0f), vec3(0.0, 0.0f, -1.0f),
    };

    azer::Vector3 pos(0.0f, 0.0f, 0.0f);
    azer::Vector3 dir(1.0f, 1.0f, 1.0f);
    dir.Normalize();
    azer::Plane plane(plane_points);
    Ray ray(pos, dir);
    float dist = plane.distance(ray);
    EXPECT_TRUE(std::abs(dist - -0.57735f) < 0.0001) << dist;
    EXPECT_TRUE(plane.belong(pos + dir * dist));
  }
}


TEST(Plane, Intersect) {
  {
    // case1
    const vec3  plane_points[] = {
      vec3(1.0, 0.0f, 0.0f), vec3(0.0, 1.0f, 0.0f), vec3(0.0, 0.0f, 1.0f),
    };
  
    azer::Plane plane(plane_points);
    Ray ray(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f).NormalizeCopy());
    vec3 inter = plane.intersect(ray);
    EXPECT_TRUE(plane.belong(inter));
    EXPECT_EQ(inter, azer::vec3(0.33333f, 0.33333f, 0.33333f));
  }

  {
    // case2
    const vec3  plane_points[] = {
      vec3(-1.0f, -1.0f, 5.0f), vec3(1.0, 1.0f, 5.0f), vec3(-1.0, 1.0f, 5.0f),
    };
  
    azer::Plane plane(plane_points);
    Ray ray(vec3(0.0f, 2.0f, -80.0f), vec3(0.0f, 0.0f, 1.0f).NormalizeCopy());
    vec3 inter = plane.intersect(ray);
    EXPECT_TRUE(plane.belong(inter));
    EXPECT_EQ(inter, azer::vec3(0.0f, 2.0f, 5.0f));
  }

  {
    // case3
    const vec3  plane_points[] = {
      vec3(-1.0f, -1.0f, 5.0f), vec3(1.0, 1.0f, 6.0f), vec3(-1.0, 1.0f, 5.0f),
    };
  
    azer::Plane plane(plane_points);
    Ray ray(vec3(0.0f, 2.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f).NormalizeCopy());
    vec3 inter = plane.intersect(ray);
    EXPECT_TRUE(plane.belong(inter));
    EXPECT_EQ(inter, azer::vec3(0.0f, 2.0f, 5.0f));
  }
}

TEST(Triangle, PointIn) {
  {
    // case1
    const vec3  plane_points[] = {
      vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
    };
    TrianglePlane plane(plane_points);
    EXPECT_TRUE(plane.IsPointIn(vec3(0.3f, 0.3f, 0.0f)));
    EXPECT_TRUE(plane.IsPointIn(vec3(0.2f, 0.2f, 0.0f)));
    EXPECT_FALSE(plane.IsPointIn(vec3(0.2f, -0.2f, 0.0f)));
    EXPECT_FALSE(plane.IsPointIn(vec3(-0.2f, 0.2f, 0.0f)));
    EXPECT_FALSE(plane.IsPointIn(vec3(0.2f, 1.02f, 0.0f)));
    EXPECT_FALSE(plane.IsPointIn(vec3(1.02f, 0.02f, 0.0f)));
    EXPECT_FALSE(plane.IsPointIn(vec3(0.2f, 0.2f, 1.0f)));
  }
}


// TODO
/**
 * 增加射线与平面平行的单元测试
 */
