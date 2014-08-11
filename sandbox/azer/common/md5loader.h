#pragma once

#include <string>
#include "base/files/file_path.h"
#include "base/strings/string_piece.h"
#include "azer/render/mesh.h"

class MD5Loader {
 public:
  MD5Loader()
      : joints_num_(0)
      , mesh_num_(0)
      , state_(kIntermid) {
  }

  bool Load(const base::FilePath& path);
  void CreateMesh(azer::Mesh* mesh);

  void dump();
 private:
  void HandleLine(const base::StringPiece& line);
  void HandleJointsLine(const base::StringPiece& line);
  void HandleMeshLine(const base::StringPiece& line);
  void HandleMetaInfo(const base::StringPiece& line);

  struct Joint {
    char name[128];
    int parent;
    float pos[3];
    float rot[4];
  };

  struct Vertex {
    float tex[2];
    int index;
    int weight_start;
    int weight_num;

    Vertex() {
      tex[0] = 0.0f; tex[1] = 0.0f;
      index = 0;
      weight_start = 0;
      weight_num = 0;
    }
  };

  struct Triangle {
    int index;
    int data[3];
  };

  struct Weight {
    int index;
    int joint;
    float bias;
    float pos[3];
  };

  struct Mesh {
    std::string tex;
    std::vector<Vertex> vertices;
    std::vector<Weight> weights;
    std::vector<Triangle> triangles;
  };

  enum State {
    kIntermid = 0,
    kJoints,
    kMesh,
    
  };

  struct VBVertex {
    azer::Vector4 pos;
    azer::Vector2 tex;
    azer::Vector3 normal;
  };

  azer::Vector3 CalcPos(Mesh* mesh, const Vertex& v);
  void ComputeNormal(VBVertex* v, int vertex_num, int32* indices, int indices_num);
  void HandleVertex(const std::string& data);
  void HandleWeight(const std::string& data);

  int joints_num_;
  int mesh_num_;
  State state_;
  base::FilePath path_;
  std::vector<Mesh*> mesh_;
  std::vector<Joint> joints_;
 public:
  static const azer::VertexDesc::Desc kVertexDescs[];
  static const int kDescNum;
};
