#pragma once

#include <string>

#include "base/basictypes.h"
#include "base/logging.h"
#include "azer/math/math.h"
#include "azer/render/render.h"

struct aiScene;
struct aiMesh;

class Mesh {
 public:
  struct Vertex {
    azer::Vector3 pos;
    azer::Vector2 tex;
    azer::Vector3 normal;
    
    Vertex(const azer::Vector3& p, 
           const azer::Vector3& norm,
           const azer::Vector2& t)
        : pos(p), tex(t), normal(norm) {}
  };

  bool LoadMesh(const std::string& filename);
  void Render();
 private:
  struct MeshEntry {
    azer::VertexDataPtr  data;
    azer::VertexBufferPtr vb;
    azer::IndicesBufferPtr ib;

    int32 vertex_num;
    int32 indices_num;
    int32 material_idx;
    MeshEntry()
        : vertex_num(0)
        , indices_num(0)
        , material_idx(-1) {
    }
    
    void Init(const std::vector<Vertex>& vertices,
              const std::vector<uint32>& indices);
  };

  void Init(const std::vector<Vertex>& vertices,
            const std::vector<uint32>& indices);
  void InitMesh(uint32 index, const aiMesh* mesh, int material_num);
  bool InitMaterials(const aiScene* scene, const std::string& filepath);

  std::vector<MeshEntry> entries_; 
  std::vector<azer::TexturePtr> textures_;
};
