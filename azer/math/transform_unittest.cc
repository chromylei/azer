#include "azer/math/math.h"
#include "gtest/gtest.h"
#include "azer/math/glm_util.h"
#include "glm/gtx/transform.hpp"

TEST(Transform, Perspective) {
  float aspect = 4.0f / 3.0f;
  azer::Matrix4 proj = azer::PerspectiveRH(azer::Degree(60.0f), aspect, 1.0, 100.0f);
  glm::mat4 glm_proj = glm::perspective(60.0f, aspect, 1.0f, 100.0f);
  ASSERT_TRUE(proj == glm_proj);
}

TEST(Transform, LookAtLH) {
}

TEST(Transform, LookAtRH) {
  {
    azer::Vector3 pos(0, 0, -1);
    azer::Vector3 lookat(0, 0, 0);
    azer::Vector3 up(0, 1, 0);
    azer::Matrix4 view_rh = LookAtRH(pos, lookat, up);
    glm::mat4 glm_mat = glm::lookAt(glm_conv(pos), glm_conv(lookat), glm_conv(up));
    ASSERT_TRUE(view_rh == glm_mat);
  }

  {
    azer::Vector3 pos(0, 0, 1);
    azer::Vector3 lookat(0, 0, -1);
    azer::Vector3 up(0, 1, 0);
    azer::Matrix4 view_rh = LookAtRH(pos, lookat, up);
    glm::mat4 glm_mat = glm::lookAt(glm_conv(pos), glm_conv(lookat), glm_conv(up));
    ASSERT_TRUE(view_rh == glm_mat);
  }

  {
    azer::Vector3 pos(0.0f, 10.0f, 18.0f);
    azer::Vector3 lookat(0.0f, 0.0f, -20.0f);
    azer::Vector3 up(0.0f, 1.0f, 0.0f);
    azer::Matrix4 view_rh = LookAtRH(pos, lookat, up);
    glm::mat4 glm_mat = glm::lookAt(glm_conv(pos), glm_conv(lookat), glm_conv(up));
    ASSERT_TRUE(view_rh == glm_mat);
  }

  {
    azer::Vector3 pos(12.0f, 8.0f, 0.0f);
    azer::Vector3 lookat(0.0f, 0.0f, 0.0f);
    azer::Vector3 up(0.0f, 1.0f, 0.0f);
    azer::Matrix4 view_rh = LookAtRH(pos, lookat, up);
    glm::mat4 glm_mat = glm::lookAt(glm_conv(pos), glm_conv(lookat), glm_conv(up));
    ASSERT_TRUE(view_rh == glm_mat);
  }

  {
    azer::Vector3 pos(-14.127034f, 3.5436306f, -0.13424602f);
    azer::Vector3 dir(0.99847746f, -0.017452411f, 0.052327991f);
    azer::Vector3 up(0.017428493f, 0.99984771f, 0.00091339089f);
    azer::Vector3 lookat = dir + pos;
    azer::Matrix4 view_dir_rh = LookDirRH(pos, dir, up);
    azer::Matrix4 view_at_rh = LookAtRH(pos, lookat, up);
    glm::mat4 glm_mat = glm::lookAt(glm_conv(pos), glm_conv(lookat), glm_conv(up));
    ASSERT_TRUE(view_dir_rh == glm_mat);
    ASSERT_TRUE(view_at_rh == view_at_rh);
  }

  {
    azer::Vector3 pos(-16.544796f, 8.0865746f, -0.26095578f);
    azer::Vector3 dir(0.94422287f, -0.32556826f, 0.049484849f);
    azer::Vector3 up(0.017428493f, 0.99984771f, 0.00091339089f);
    azer::Vector3 lookat = dir + pos;
    azer::Matrix4 view_dir_rh = LookDirRH(pos, dir, up);
    azer::Matrix4 view_at_rh = LookAtRH(pos, lookat, up);
    glm::mat4 glm_mat = glm::lookAt(glm_conv(pos), glm_conv(lookat), glm_conv(up));
    ASSERT_TRUE(view_dir_rh == glm_mat);
    ASSERT_TRUE(view_at_rh == view_at_rh);
  }
}
