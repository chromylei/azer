#pragma once


#include <d3d11.h>
#include <xnamath.h>
#include "azer/render/render.h"
#include "sandbox/azer/common/loadobj.h"
#include "sandbox/azer/common/camera.h"

class Leaf {
 public:
  void Init();
  void Render(const azer::Light& light,
              XMMATRIX* matworld, XMMATRIX *matwvp, int num);

  azer::VertexBuffer* VertexBuffer() const { return vb_.get();}
 private:
  void InitShader();

  struct LeafVertex {
    azer::Vector3 position;
    azer::Vector2 tex;
    azer::Vector3 normal;

    LeafVertex(const azer::Vector3& p, const azer::Vector2& t,
               const azer::Vector3& n)
        : position(p), tex(t), normal(n) {
    }
  };

  
  azer::TexturePtr tex_;
  azer::VertexBufferPtr vb_;
  azer::IndicesBufferPtr ib_;
  azer::BlendingPtr blending_;

  azer::TechniquePtr leaf_tech_;
  azer::GpuProgramPtr leaf_vs_;
  azer::GpuProgramPtr leaf_fs_;
  azer::GpuConstantsTablePtr vs_table_;
  azer::GpuConstantsTablePtr ps_table_;

  static const azer::VertexDesc::Desc kVertexDescs[];
};

class TreeMesh {
 public:
  void Init();
  void Render(const Camera& camera, const azer::Light& light);
 private:
  void InitLeaf();
  Leaf leaf_;
  azer::Mesh tree_;
  azer::TechniquePtr tree_tech_;
  azer::GpuProgramPtr tree_vs_;
  azer::GpuProgramPtr tree_fs_;

  azer::GpuConstantsTablePtr vs_table_;
  azer::GpuConstantsTablePtr ps_table_;

 public:
  static const int kLeafPerTree = 500;
 private:
  XMMATRIX leaves_world_[kLeafPerTree];
  XMMATRIX leaves_wvp_[kLeafPerTree];
};
