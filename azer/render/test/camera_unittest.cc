#include "azer/render/render.h"
#include "gtest/gtest.h"
#include "azer/math/glm_util.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

using azer::Degree;
using azer::Radians;
using azer::Vector3;
using azer::Matrix4;

TEST(Camera, BasicDirection) {
  azer::Camera camera;
  ASSERT_EQ(camera.right(), Vector3(1.0, 0.0, 0.0));
  ASSERT_EQ(camera.up(), Vector3(0.0, 1.0, 0.0));
  ASSERT_EQ(camera.direction(), Vector3(0.0, 0.0, -1.0));
}

TEST(Camera, DefaultViewMatrix) {
  azer::Camera camera;
  Matrix4 mat = camera.GetViewMatrix();
  ASSERT_EQ(mat, Matrix4::kIdentity);
}

TEST(Camera, GetViewMatrix) {
  {
    azer::Vector3 pos(-16.544796f, 8.0865746f, -0.26095578f);
    azer::Vector3 dir(0.94422287f, -0.32556826f, 0.049484849f);
    azer::Vector3 up(0.017428493f, 0.99984771f, 0.00091339089f);
    azer::Vector3 lookat = dir + pos;
    azer::Matrix4 view_dir_rh = LookDirRH(pos, dir, up);
    azer::Matrix4 view_at_rh = LookAtRH(pos, lookat, up);
    azer::Camera camera(pos, lookat, up);
    azer::Matrix4 view_gen = camera.GetViewMatrix();
    glm::mat4 glm_mat = glm::lookAt(glm_conv(pos), glm_conv(lookat), glm_conv(up));
    ASSERT_TRUE(view_dir_rh == glm_mat);
    ASSERT_TRUE(view_at_rh == view_at_rh);
    ASSERT_TRUE(view_at_rh == view_gen);
  }
}

TEST(Camera, Walk1) {
  azer::Vector3 pos(0, 0, 1);
  azer::Vector3 lookat(0, 0, 0);
  azer::Vector3 up(0, 1, 0);
  azer::Camera camera;
  camera.reset(pos, lookat, up);
  azer::Vector4 world_pos(0, 0, 0, 1);
  camera.Update();

  {
    azer::Matrix4 view = camera.GetViewMatrix();
    azer::Vector4 camera_pos = view * world_pos;
    ASSERT_EQ(camera_pos, azer::Vector4(0, 0, -1, 1));
  }

  {
    // walk along -z_axis with 1.0 step
    camera.walk(1.0);
    camera.Update();
    azer::Matrix4 view = camera.GetViewMatrix();
    azer::Vector4 camera_pos = view * world_pos;
    ASSERT_EQ(camera_pos, azer::Vector4(0, 0, 0, 1));
  }
}

TEST(Camera, MatrixAndQuaternion) {
  /*
   * 由 Quaternion 和 Matrix 生成的 Camera 矩阵应该是一致的
   */
  azer::Vector3 pos(0, 0, -1);
  azer::Vector3 lookat(0, 0, 0);
  azer::Vector3 up(0, 1, 0);
  azer::Camera camera1;
  camera1.reset(pos, lookat, up);
  azer::Matrix4 view = camera1.GetViewMatrix();
  azer::Matrix4 view_rh = LookAtRH(pos, lookat, up);
  ASSERT_EQ(view, view_rh);
}

TEST(Camera, Pitch) {
  {
    azer::Camera camera;
    camera.reset(Vector3(-1.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f),
                 Vector3(0.0f, 1.0f, 0.0f));
    EXPECT_EQ(camera.right(), Vector3(-0.70710671f, 0.0f, 0.70710683f));
    EXPECT_EQ(camera.up(), Vector3(0.0f, 1.0f, 0.0f));
    EXPECT_EQ(camera.direction(), Vector3(0.70710683f, 0.0f, 0.70710671f));
    camera.pitch(azer::Degree(90.0f));
    EXPECT_EQ(camera.right(), Vector3(-0.70710671f, 0.0f, 0.70710683f));
    EXPECT_EQ(camera.up(), Vector3(-0.70710683f, 0.0f, -0.70710671f));
    EXPECT_EQ(camera.direction(), Vector3(0.0f, 1.0f, 0.0f));

    camera.pitch(azer::Degree(90.0f));
  }
}

TEST(Camera, Rotate) {
  {
    azer::Camera camera;
    camera.rotate(Vector3(0.0f, 1.0f, 0.0f), Radians(azer::kPI / 2.0f));
    ASSERT_EQ(camera.right(), Vector3(0.0f, 0.0, -1.0));
    ASSERT_EQ(camera.up(), Vector3(0.0, 1.0, 0.0));
    ASSERT_EQ(camera.direction(), Vector3(-1.0f, 0.0f, 0.0f));
  }
}

TEST(Camera, ZeroSequence) {
  azer::Camera camera;
  azer::Vector3 pos(12.0f, 8.0f, 0.0f);
  azer::Vector3 lookat(0.0f, 8.0f, 0.0f);
  azer::Vector3 up(0.0f, 1.0f, 0.0f);
  camera.reset(pos, lookat, up);
  camera.yaw(azer::Degree(90.0));
  ASSERT_EQ(camera.position(), pos);
  camera.pitch(azer::Degree(-15.0));
  camera.pitch(azer::Degree(15.0));
  ASSERT_EQ(camera.direction(), azer::Vector3(0.0f, 0.0f, 1.0f));
  camera.yaw(azer::Degree(-15.0f));
  camera.yaw(azer::Degree(30.0f));
  camera.yaw(azer::Degree(-15.0f));
  ASSERT_EQ(camera.direction(), azer::Vector3(0.0f, 0.0f, 1.0f));
  camera.roll(azer::Degree(-15.0f));
  camera.roll(azer::Degree(30.0f));
  camera.roll(azer::Degree(-15.0f));
  ASSERT_EQ(camera.direction(), azer::Vector3(0.0f, 0.0f, 1.0f));
}

TEST(Camera, ZeroSequence2) {
  azer::Camera camera;
  azer::Vector3 pos(12.0f, 8.0f, 0.0f);
  azer::Vector3 lookat(0.0f, 8.0f, 0.0f);
  azer::Vector3 up(0.0f, 1.0f, 0.0f);
  camera.reset(pos, lookat, up);
  
  glm::quat quat1 = glm::angleAxis(90.0f, glm::vec3(0, 1, 0));
  glm::vec3 v1 = quat1 * glm_conv(camera.direction());
  camera.yaw(azer::Degree(90.0));
  ASSERT_TRUE(v1 == camera.direction());

  
  glm::quat quat2 = glm::angleAxis(-15.0f, glm_conv(camera.right()));
  glm::vec3 v2 = quat2 * glm_conv(camera.direction());
  camera.pitch(azer::Degree(-15.0));
  EXPECT_TRUE(v2 == camera.direction());

  glm::quat quat3 = glm::angleAxis(-90.0f, glm_conv(camera.up()));
  glm::vec3 v3 = quat3 * glm_conv(camera.direction());
  camera.yaw(azer::Degree(-90.0));
  EXPECT_TRUE(v3 == camera.direction());


  glm::quat quat4 = glm::angleAxis(15.0f, glm_conv(camera.right()));
  glm::vec3 v4 = quat4 * glm_conv(camera.direction());
  camera.pitch(azer::Degree(15.0));
  EXPECT_TRUE(v4 == camera.direction());
}
