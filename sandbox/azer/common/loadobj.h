#pragma once

#include "azer/render/render.h"
#include "azer/render/mesh.h"

class ObjMeshLoader {
 public:
  ObjMeshLoader();
  bool Load(const base::FilePath& path, azer::Mesh *mesh);

  static const azer::VertexDesc::Desc kVertexDescs[];
  static const int kDescNum;

  void dump();

  struct Vertex {
    azer::Vector4 position;
    azer::Vector2 tex;
    azer::Vector3 normal;
    azer::Vector3 tangent;

    Vertex(const azer::Vector4& p, const azer::Vector2& t,
           const azer::Vector3& n)
        : position(p), tex(t), normal(n) {
    }
  };
 private:
  struct ObjFace {
    int v, vt, vn;
    ObjFace(int v, int vt = -1, int vn = -1) {
      this->v = v;
      this->vt = vt;
      this->vn = vn;
    }

    ObjFace() : v(-1), vt(-1), vn(-1) {
    }

    friend std::ostream& operator << (std::ostream& os, const ObjFace& f) {
      os << "(" << f.v << ", " << f.vt << ", " << f.vn << ")";
      return os;
    }
  };

  struct Group {
    std::vector<ObjFace> f;
    azer::MaterialPtr mtrl;
    std::string name;    // group name
  };

  std::vector<azer::vec3> v_;
  std::vector<azer::vec3> vn_;
  std::vector<azer::vec3> vt_;

  void DumpGroup(const Group& g);
  void HandleLine(const std::string& line);
  void LoadMaterial();
  void HandleMaterialLine(const std::string& line);
  void TransGroup(Group* sub, azer::Mesh::SubMesh* asub);
  void InitSubMesh(azer::Mesh::SubMesh* asub);
  void ParseMaterial(const std::string& path);
  void ParseFace(const std::string& val);

  azer::MaterialPtr curr_mtrl_;
  ::base::FilePath path_;
  Group* curr_;
  std::vector<Group*> entries_;
  std::map<std::string, azer::MaterialPtr> materials_;
};
