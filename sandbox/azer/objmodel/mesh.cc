#include "azer/math/math.h"
#include "sandbox/azer/objmodel/mesh.h"
#include "base/strings/utf_string_conversions.h"

#include "assimp/postprocess.h"
#include "assimp/version.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"


bool Mesh::LoadMesh(const std::string& filename) {
  Assimp::Importer importer;
  uint32 f = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
  const aiScene* pScene = importer.ReadFile(filename.c_str(), f);
  if (!pScene) {
    LOG(ERROR) << "failed to load mesh: " <<  filename;
    return false;
  }

  entries_.resize(pScene->mNumMeshes);
  textures_.resize(pScene->mNumMaterials);

  for (uint32 i = 0; i < entries_.size(); ++i) {
    const aiMesh* paiMesh = pScene->mMeshes[i];
    InitMesh(i, paiMesh, pScene->mNumTextures);
  }

  return InitMaterials(pScene, filename);
}

void Mesh::InitMesh(uint32 index, const aiMesh* mesh, int material_num) {
  if (material_num > 0) {
    entries_[index].material_idx = mesh->mMaterialIndex;
  }
  std::vector<Vertex> vertices;
  std::vector<uint32> indices;
  const aiVector3D zero(0.0, 0.0, 0.0);
  fprintf(stderr, "vertx num: %d\n", mesh->mNumVertices);
  for (uint32 i = 0; i < mesh->mNumVertices; ++i) {
    const aiVector3D& pos = mesh->mVertices[i];
    const aiVector3D& norm = mesh->mNormals[i];
    const aiVector3D& texcord = mesh->HasTextureCoords(0) ?
        (mesh->mTextureCoords[0][i]) : zero;
    Mesh::Vertex vertex(azer::Vector3(pos.x, pos.y, pos.z),
                        azer::Vector3(norm.x, norm.y, norm.z),
                        azer::Vector2(texcord.x, texcord.y)
                        );
    vertices.push_back(vertex);
  }

  for (uint32 i = 0; i < mesh->mNumFaces; ++i) {
    const aiFace& face = mesh->mFaces[i];
    assert(face.mNumIndices == 3);
    indices.push_back(face.mIndices[0]);
    indices.push_back(face.mIndices[1]);
    indices.push_back(face.mIndices[2]);
  }

  entries_[index].Init(vertices, indices);
}

using azer::VertexDesc;
const VertexDesc::Desc kVertexDescs[] = {
  {"POSITION", VertexDesc::kPosition, azer::kVec3},
  {"TEXCOORD", VertexDesc::kTexCoord, azer::kVec2},
  {"NORMAL", VertexDesc::kTexCoord, azer::kVec3},
};

void Mesh::MeshEntry::Init(const std::vector<Vertex>& v,
                           const std::vector<uint32>& i) {
  using namespace azer;
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  azer::VertexDescPtr vdesc(new azer::VertexDesc(kVertexDescs,
      ARRAYSIZE(kVertexDescs)));
  data.reset(new azer::VertexData(vdesc, v.size() * sizeof(Vertex)));
  memcpy(data->pointer(), (uint8*)&v[0], sizeof(Vertex) * sizeof(v));
  vb.reset(rs->CreateVertexBuffer(azer::VertexBuffer::Options(), data));

  azer::IndicesDataPtr idata(new IndicesData(i.size(),
                                             IndicesData::kUint32,
                                             IndicesData::kMainMemory));
  memcpy(idata->pointer(), (uint8*)&i[0], sizeof(uint32) * i.size());
  ib.reset(rs->CreateIndicesBuffer(azer::IndicesBuffer::Options(), idata));

  vertex_num = v.size();
  indices_num = i.size();
}

void Mesh::Render() {
  azer::RenderSystem* rs = azer::RenderSystem::Current();
  for (auto iter = entries_.begin(); iter != entries_.end(); ++iter) {
    if (iter->material_idx > 0) {
      rs->UseTexture(azer::kPixelStage, 0, textures_[iter->material_idx].get());
    }
    rs->Render(iter->vb.get(), iter->ib.get(), azer::kTriangleList,
               iter->indices_num);
  }
}

bool Mesh::InitMaterials(const aiScene* scene, const std::string& filepath) {
  std::string::size_type index = filepath.find_last_of("/");
  std::string dir;
  if (index == std::string::npos) {
    dir = ".";
  } else if (index == 0) {
    dir = "/";
  } else {
    dir = filepath.substr(0, index);
  }

  for (uint32 i = 0; i < scene->mNumMaterials; ++i) {
    const aiMaterial* material = scene->mMaterials[i];
    textures_[i] = NULL;
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
      aiString path;
      if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL,
                               NULL, NULL) == AI_SUCCESS) {
        std::string fullpath = dir + "/" + path.data;
        azer::RenderSystem* rs = azer::RenderSystem::Current();
        azer::TexturePtr texture(rs->CreateTextureFromFile(
            azer::Texture::k2D, base::FilePath(ASCIIToWide(filepath.c_str()))));
        CHECK(texture.get() != NULL);
        textures_.push_back(texture);
      }
    }
  }

  return true;
}
