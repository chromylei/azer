#include "azer/azer.h"
#include "azer/render/test/util.h"
#include "gtest/gtest.h"

class ResourceManagerTest : public testing::Test {
 public:
  virtual void SetUp() OVERRIDE {
    render_system_ = azer::testing::CreateD3D11RenderSystemForTest();
    fs_ptr_.reset(azer::FileSystem::create(
        azer::FileSystem::kNativeFS,
        ::base::FilePath(AZER_LITERAL("azer/resources/testdata"))));
  };

  virtual void TearDown() OVERRIDE {
    render_system_->Release();
  };
 protected:
  azer::RenderSystem* render_system_;
  azer::FileSystemPtr fs_ptr_;
};

TEST_F(ResourceManagerTest, MaterialBase) {
  azer::ResourceManager rsmgr(fs_ptr_);
  azer::MaterialPtr mtrl_ptr = rsmgr.GetMaterial(
      AZER_LITERAL("material/base.mtrl:base"), render_system_);
  ASSERT_TRUE(mtrl_ptr.get() != NULL);
  ASSERT_EQ(mtrl_ptr->attributes().transparency, 1);
  ASSERT_EQ(mtrl_ptr->attributes().specular_power, 20.0f);
  ASSERT_EQ(mtrl_ptr->attributes().optical_density, 2);
  ASSERT_EQ(mtrl_ptr->attributes().illumination, 3);
  ASSERT_EQ(mtrl_ptr->attributes().ambient, azer::Vector4(0.1f, 0.2f, 0.3f, 1.0f));
  ASSERT_EQ(mtrl_ptr->attributes().diffuse, azer::Vector4(0.4f, 0.5f, 0.6f, 1.0f));
  ASSERT_EQ(mtrl_ptr->diffuse_tex().size(), 1u);
  ASSERT_EQ(mtrl_ptr->specular_tex().size(), 1u);
  ASSERT_EQ(mtrl_ptr->normal_tex().size(), 1u);
  ASSERT_EQ(mtrl_ptr->alpha_tex().size(), 1u);  
}

TEST_F(ResourceManagerTest, VertexDescBase) {
  azer::ResourceManager rsmgr(fs_ptr_);
  azer::VertexDescPtr ptr = rsmgr.GetVertexDesc(AZER_LITERAL("vdesc/base.vdesc:base1"));
  ASSERT_TRUE(ptr.get() != NULL);
  const azer::VertexDesc::Desc* desc = ptr->descs();
  ASSERT_EQ(ptr->element_num(), 3);
  ASSERT_STREQ(desc[0].name, "POSITION");
  ASSERT_EQ(desc[0].type, azer::kVec3);
  ASSERT_STREQ(desc[1].name, "TEXCOORD");
  ASSERT_EQ(desc[1].type, azer::kVec2);
  ASSERT_STREQ(desc[2].name, "NORMAL");
  ASSERT_EQ(desc[2].type, azer::kVec3);

  azer::VertexDescPtr ptr2 = rsmgr.GetVertexDesc(AZER_LITERAL("vdesc/base.vdesc:base2"));
  ASSERT_TRUE(ptr2.get() != NULL);
  desc = ptr2->descs();
  ASSERT_EQ(ptr->element_num(), 3);
  ASSERT_STREQ(desc[0].name, "POSITION");
  ASSERT_EQ(desc[0].type, azer::kVec4);
  ASSERT_STREQ(desc[1].name, "TEXCOORD");
  ASSERT_EQ(desc[1].type, azer::kVec2);
  ASSERT_STREQ(desc[2].name, "NORMAL");
  ASSERT_EQ(desc[2].type, azer::kVec4);
}

TEST_F(ResourceManagerTest, InvalidVertexDesc) {
}

TEST_F(ResourceManagerTest, Light) {
  azer::ResourceManager rsmgr(fs_ptr_);
  azer::LightPtr ptr1 = rsmgr.GetLight(AZER_LITERAL("scene/scene1.light:light1"));
  ASSERT_EQ(ptr1->type(), azer::Light::kDirectionalLight);
  ASSERT_EQ(ptr1->ambient(), azer::vec4(0.1f, 0.21f, 0.41f, 1.0f));
  ASSERT_EQ(ptr1->diffuse(), azer::vec4(0.5f, 0.6f, 0.7f, 1.0f));
  ASSERT_EQ(ptr1->directional(), azer::vec4(0.0f, 1.0f, 0.0f, 0.0f));
  
                                          
  azer::LightPtr ptr2 = rsmgr.GetLight(AZER_LITERAL("scene/scene1.light:light2"));
  ASSERT_EQ(ptr2->type(), azer::Light::kPointLight);
  ASSERT_EQ(ptr2->ambient(), azer::vec4(0.3f, 0.3f, 0.3f, 1.0f));
  ASSERT_EQ(ptr2->diffuse(), azer::vec4(0.4f, 0.4f, 0.4f, 1.0f));
  ASSERT_EQ(ptr2->position(), azer::vec4(0.0f, 8.0f, -8.0f, 1.0f));
  ASSERT_EQ(ptr2->range(), 1000.0f);
  ASSERT_EQ(ptr2->attenuation().cvalue, 1.0f);
  ASSERT_EQ(ptr2->attenuation().linear, 2.0f);
  ASSERT_EQ(ptr2->attenuation().expr, 3.0f);
  
  azer::LightPtr ptr3 = rsmgr.GetLight(AZER_LITERAL("scene/scene1.light:light3"));
  ASSERT_EQ(ptr3->type(), azer::Light::kSpotLight);
  ASSERT_EQ(ptr3->ambient(), azer::vec4(0.3f, 0.4f, 0.5f, 1.0f));
  ASSERT_EQ(ptr3->diffuse(), azer::vec4(0.5f, 0.8f, 0.7f, 1.0f));
  ASSERT_EQ(ptr3->position(), azer::vec4(0.0f, -8.0f, 8.0f, 1.0f));
  ASSERT_EQ(ptr3->directional(), azer::vec4(0.0f, 1.0f, 0.0f, 0.0f));
  ASSERT_EQ(ptr3->range(), 3000.0f);
  ASSERT_EQ(ptr3->attenuation().cvalue, 10.0f);
  ASSERT_EQ(ptr3->attenuation().linear, 20.0f);
  ASSERT_EQ(ptr3->attenuation().expr, 30.0f);
}


TEST_F(ResourceManagerTest, SceneNode) {
  using azer::SceneNode;
  azer::ResourceManager rsmgr(fs_ptr_);
  azer::ScenePtr ptr = rsmgr.GetScene(AZER_LITERAL("scene/scene1.sce:scene1"),
                                      render_system_);
  ASSERT_EQ(ptr->first_child()->name(), AZER_LITERAL("node1"));
  ASSERT_EQ(ptr->first_child()->type(), SceneNode::kScene);
  ASSERT_EQ(ptr->last_child()->name(), AZER_LITERAL("node2"));
  ASSERT_EQ(ptr->last_child()->type(), SceneNode::kMesh);
  ASSERT_TRUE(ptr->first_child()->HasChildren());
  ASSERT_FALSE(ptr->last_child()->HasChildren());
}


/**
 * lack texture, mesh, light or some other resources
 * 
 */
TEST_F(ResourceManagerTest, ResourceLack) {
}

TEST_F(ResourceManagerTest, Technique) {
  using azer::SceneNode;
  azer::ResourceManager rsmgr(fs_ptr_);
  azer::TechniquePtr ptr = rsmgr.GetTechnique(
      AZER_LITERAL("shader/coordinategrideffect.tech"),  render_system_);
  ASSERT_TRUE(ptr.get() != NULL);
}
