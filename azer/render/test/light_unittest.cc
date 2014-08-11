#include "azer/render/light.h"
#include "azer/math/math.h"
#include "base/basictypes.h"
#include "base/logging.h"
#include "gtest/gtest.h"

using namespace azer;

TEST(Light, DirectionalLight) {
  vec4 dir(1.0f, 0.0f, 0.0f, 0.0f);
  vec4 ambient(0.2f, 0.2f, 0.2f, 0.2f);
  vec4 diffuse(0.1f, 0.1f, 0.1f, 1.0f);
  Light light(Light::kDirectionalLight);
  light.set_directional(dir);
  light.set_ambient(ambient);
  light.set_diffuse(diffuse);

  ASSERT_EQ(light.directional(), dir);
  ASSERT_EQ(light.ambient(), ambient);
  ASSERT_EQ(light.diffuse(), diffuse);
  ASSERT_DEATH(light.set_range(0.0), "");
  ASSERT_DEATH(light.set_cone(0.0), "");
  ASSERT_DEATH(light.position(), "");
  ASSERT_DEATH(light.range(), "");
  ASSERT_DEATH(light.attenuation(), "");
  ASSERT_DEATH(light.cone(), "");
}

TEST(Light, PointLight) {
  float range = 1000.0f;
  vec4 position(1.0f, 0.0f, 0.0f, 1.0f);
  vec4 ambient(0.2f, 0.2f, 0.2f, 0.2f);
  vec4 diffuse(0.1f, 0.1f, 0.1f, 1.0f);
  Light light(Light::kPointLight);
  light.set_position(position);
  light.set_ambient(ambient);
  light.set_diffuse(diffuse);
  light.set_range(range);
  
  ASSERT_EQ(light.position(), position);
  ASSERT_EQ(light.ambient(), ambient);
  ASSERT_EQ(light.diffuse(), diffuse);
  ASSERT_EQ(light.range(), range);
  ASSERT_DEATH(light.directional(), "");
  ASSERT_DEATH(light.set_cone(0.0), "");
  ASSERT_DEATH(light.cone(), "");
}

TEST(Light, SpotLight) {
  float range = 1000.0f, cone = 20.0f;
  
  vec4 directional(1.8f, 3.0f, 0.8f, 1.0f);
  vec4 position(1.0f, 0.0f, 0.0f, 1.0f);
  vec4 ambient(0.2f, 0.2f, 0.2f, 0.2f);
  vec4 diffuse(0.1f, 0.1f, 0.1f, 1.0f);
  Light light(Light::kSpotLight);
  light.set_position(position);
  light.set_directional(directional);
  light.set_ambient(ambient);
  light.set_diffuse(diffuse);
  light.set_range(range);
  light.set_cone(cone);
  
  ASSERT_EQ(light.position(), position);
  ASSERT_EQ(light.directional(), directional);
  ASSERT_EQ(light.ambient(), ambient);
  ASSERT_EQ(light.diffuse(), diffuse);
  ASSERT_EQ(light.range(), range);
  ASSERT_EQ(light.cone(), cone);
}
