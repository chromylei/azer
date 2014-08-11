#include "sandbox/azer/instance/tree_mesh.h"

#include "base/file_util.h"


const azer::VertexDesc::Desc Leaf::kVertexDescs[] = {
  {"POSITION", azer::VertexDesc::kPosition, azer::kVec3},
  {"TEXCOORD", azer::VertexDesc::kTexCoord, azer::kVec2},
  {"NORMAL", azer::VertexDesc::kTexCoord, azer::kVec3},
};

void Leaf::Init() {
  using namespace azer;
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  VertexDescPtr desc(new VertexDesc(kVertexDescs, ARRAYSIZE(kVertexDescs)));
  VertexDataPtr data(new VertexData(desc, 4));
  LeafVertex* vertex = (LeafVertex*)data->pointer();
  *vertex++ = LeafVertex(Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0f, 1.0f),
                        Vector3(0.0f, 0.0f, -1.0f));
  *vertex++ = LeafVertex(Vector3(-1.0f,  1.0f, -1.0f), Vector2(0.0f, 0.0f),
                         Vector3(0.0f, 0.0f, -1.0f));
  *vertex++ = LeafVertex(Vector3( 1.0f,  1.0f, -1.0f), Vector2(1.0f, 0.0f),
                         Vector3(0.0f, 0.0f, -1.0f));
  *vertex++ = LeafVertex(Vector3( 1.0f, -1.0f, -1.0f), Vector2(1.0f, 1.0f),
                         Vector3(0.0f, 0.0f, -1.0f));

  int32 indices[] = { 0,  1,  2, 0,  2,  3,  };
  IndicesDataPtr idata(new IndicesData(6, IndicesData::kUint32,
                                       IndicesData::kMainMemory));
  memcpy(idata->pointer(), indices, sizeof(indices));
  vb_.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data));
  ib_.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), idata));

  tex_.reset(rs->CreateTextureFromFile(
      azer::Texture::k2D, base::FilePath(TEXT("sandbox/azer/media/leaf.png"))));

  {
  azer::Blending::Desc desc;
  desc.src = azer::Blending::kSrcInvAlpha;
  desc.dest = azer::Blending::kSrcAlpha;
  desc.oper = azer::Blending::kAdd;
  desc.src_alpha = azer::Blending::kSrcInvAlpha;
  desc.dest_alpha = azer::Blending::kSrcAlpha;
  desc.alpha_oper = azer::Blending::kAdd;
  blending_.reset(rs->CreateBlending(desc));
  }

  InitShader();
}

void Leaf::InitShader() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  std::string leaf_vs_prog, leaf_ps_prog;
  ::base::FilePath vertex_shader_path(TEXT("sandbox/azer/instance/leaf.vs.hlsl"));
  ::base::FilePath pixel_shader_path(TEXT("sandbox/azer/instance/leaf.ps.hlsl"));
  CHECK(::base::ReadFileToString(vertex_shader_path, &leaf_vs_prog));
  CHECK(::base::ReadFileToString(pixel_shader_path, &leaf_ps_prog));
  leaf_vs_.reset(rs->CreateVertexGpuProgram(VertexBuffer()->desc(),
                                            leaf_vs_prog));
  leaf_fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, leaf_ps_prog));
  leaf_tech_.reset(rs->CreateEffect());
  leaf_tech_->AddGpuProgram(leaf_vs_);
  leaf_tech_->AddGpuProgram(leaf_fs_);
  leaf_tech_->Finalize();

  azer::GpuConstantsTable::Desc dvs_desc[] = {
    azer::GpuConstantsTable::Desc("WVP", azer::GpuConstantsType::kMatrix4,
                                  TreeMesh::kLeafPerTree),
    azer::GpuConstantsTable::Desc("WORLD", azer::GpuConstantsType::kMatrix4,
                                  TreeMesh::kLeafPerTree),
  };
  vs_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(dvs_desc), dvs_desc));

  azer::GpuConstantsTable::Desc pvs_desc[] = {
    azer::GpuConstantsTable::Desc("light",
                                  azer::GpuConstantsType::kDirectionalLight, 1),
  };
  ps_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(pvs_desc), pvs_desc));
}

void Leaf::Render(const azer::Light& light,
                  XMMATRIX* matworld, XMMATRIX *matwvp, int num) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  // blending_->Use(NULL);
  leaf_tech_->Use();
  ps_table_->SetLight(0, light);
  vs_table_->SetValue(0, matwvp, num * sizeof(XMMATRIX));
  vs_table_->SetValue(1, matworld, num * sizeof(XMMATRIX));
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());
  rs->UseConstantsTable(azer::kPixelStage, ps_table_.get());
  vs_table_->flush();
  ps_table_->flush();

  rs->UseTexture(azer::kPixelStage, 0, tex_.get());
  rs->RenderInstanced(1000, vb_.get(), ib_.get(), azer::kTriangleList, 6);
}


void TreeMesh::Init() {
  InitLeaf();

  ObjMeshLoader objloader1;
  CHECK(objloader1.Load(base::FilePath(TEXT("sandbox/azer/media/tree.obj")),
                        &tree_)) << "Failed to load obj";

  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::GpuConstantsTable::Desc dvs_desc[] = {
    azer::GpuConstantsTable::Desc("WVP", azer::GpuConstantsType::kMatrix4, 1),
    azer::GpuConstantsTable::Desc("WORLD", azer::GpuConstantsType::kMatrix4, 1),
  };
  vs_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(dvs_desc), dvs_desc));

  azer::GpuConstantsTable::Desc pvs_desc[] = {
    azer::GpuConstantsTable::Desc("light",
                                  azer::GpuConstantsType::kDirectionalLight, 1),
  };
  ps_table_.reset(rs->CreateGpuConstantsTable(ARRAYSIZE(pvs_desc), pvs_desc));

  std::string tree_vs_prog, tree_ps_prog;
  ::base::FilePath vertex_shader_path(TEXT("sandbox/azer/instance/tree.vs.hlsl"));
  ::base::FilePath pixel_shader_path(TEXT("sandbox/azer/instance/tree.ps.hlsl"));
  CHECK(::base::ReadFileToString(vertex_shader_path, &tree_vs_prog));
  CHECK(::base::ReadFileToString(pixel_shader_path, &tree_ps_prog));
  tree_vs_.reset(rs->CreateVertexGpuProgram(leaf_.VertexBuffer()->desc(),
                                       tree_vs_prog));
  tree_fs_.reset(rs->CreateGpuProgram(azer::kPixelStage, tree_ps_prog));
  tree_tech_.reset(rs->CreateEffect());
  tree_tech_->AddGpuProgram(tree_vs_);
  tree_tech_->AddGpuProgram(tree_fs_);
  tree_tech_->Finalize();
}

void TreeMesh::Render(const Camera& camera, const azer::Light& light) {
  azer::RenderSystem* rs = azer::RenderSystem::Current();

  XMMATRIX world = XMMatrixScaling(1.0f, 1.0f, 1.0f);
  XMMATRIX WVP = world * camera.view() * camera.projection();
  XMMATRIX twvp = XMMatrixTranspose(WVP);
  XMMATRIX tworld = XMMatrixTranspose(world);

  tree_tech_->Use();
  vs_table_->SetValue(0, (void*)&twvp, sizeof(twvp));
  vs_table_->SetValue(1, (void*)&tworld, sizeof(tworld));
  ps_table_->SetLight(0, light);
  rs->UseConstantsTable(azer::kVertexStage, vs_table_.get());
  rs->UseConstantsTable(azer::kPixelStage, ps_table_.get());
  vs_table_->flush();
  ps_table_->flush();
  tree_.Render(rs);

  
  for (int i = 0; i < kLeafPerTree; ++i) {
    leaves_wvp_[i] = XMMatrixTranspose(
        XMMatrixTranspose(leaves_world_[i]) * camera.view() * camera.projection());
  }

  leaf_.Render(light, leaves_world_, leaves_wvp_, kLeafPerTree);
}

void TreeMesh::InitLeaf() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  leaf_.Init();
  XMFLOAT3 fTPos;
  XMMATRIX rotationMatrix;
  XMMATRIX tempMatrix;
  for (int i = 0; i < kLeafPerTree; ++i) {
    // Value between 0 and 4 PI (two circles, makes it slightly more mixed)
    float rotX =(rand() % 2000) / 500.0f; 
    float rotY = (rand() % 2000) / 500.0f;
    float rotZ = (rand() % 2000) / 500.0f;

    // the rand() function is slightly more biased towards lower numbers,
    // which would make the center of the leaf "mass" be more dense with
    // leaves than the outside of the "sphere" of leaves we are making.
    // We want the outside of the "sphere" of leaves to be more dense than
    // the inside, so the way we do this is getting a distance value between
    // 0 and 4, we then subtract that value from 6, so that the very center
    // does not have any leaves. then below you can see we are checking to
    // see if the distance is greater than 4 (because the tree branches are
    // approximately 4 units radius from the center of the tree). If the distance
    // is greater than 4, then we set it at 4, which will make the edge of the
    // "sphere" of leaves more densly populated than the center of the leaf mass
    float distFromCenter = 6.0f - ((rand() % 1000) / 250.0f);

    if(distFromCenter > 4.0f)
      distFromCenter = 4.0f;

    // Now we create a vector with the length of distFromCenter, by simply setting
    // it's x component as distFromCenter. We will now rotate the vector, which
    // will give us the "sphere" of leaves after we have rotated all the leaves.
    // We do not want a perfect sphere, more like a half sphere to cover the
    // branches, so we check to see if the y value is less than -1.0f (giving us
    // slightly more than half a sphere), and if it is, negate it so it is reflected
    // across the xz plane
    XMVECTOR tempPos = XMVectorSet(distFromCenter, 0.0f, 0.0f, 0.0f);
    rotationMatrix = XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ);
    tempPos = XMVector3TransformCoord(tempPos, rotationMatrix );

    if(XMVectorGetY(tempPos) < -1.0f)
      tempPos = XMVectorSetY(tempPos, -XMVectorGetY(tempPos));

    // Now we create our leaves "tree" matrix (this is not the leaves
    // "world matrix", because we are not defining the leaves position,
    // orientation, and scale in world space, but instead in "tree" space
    XMStoreFloat3(&fTPos, tempPos);

    XMMATRIX Scale = XMMatrixScaling( 0.25f, 0.25f, 0.25f );
    XMMATRIX Translation = XMMatrixTranslation(fTPos.x, fTPos.y + 8.0f, fTPos.z );
   tempMatrix = Scale * rotationMatrix * Translation;

    // To make things simple, we just store the matrix directly into our
    // cbPerInst structure
    leaves_world_[i] = XMMatrixTranspose(tempMatrix);
  }
}
