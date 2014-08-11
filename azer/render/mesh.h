#pragma once

#include "azer/base/render_export.h"
#include "azer/render/bounding_volumn.h"
#include "azer/render/cullable.h"
#include "azer/render/indices_buffer.h"
#include "azer/render/material.h"
#include "azer/render/renderable.h"
#include "azer/render/vertex_data.h"
#include "azer/render/vertex_buffer.h"

namespace azer {

class Effect;
class Renderer;

class AZER_EXPORT Mesh : public Renderable {
 public:
  Mesh() : effect_(NULL) {}
  enum GroupType {
    kVertex,
    kIndices,
  };

  struct Group {
    int32 first;
    int32 num;
    GroupType type;
    PrimitiveTopology primitive;
    MaterialPtr mtrl_ptr;

    Group()
        : first(0)
        , num(0)
        , type(kIndices) {}
  };

  typedef std::vector<Group> GroupVec;
  struct SubMesh {
    VertexBufferPtr vb_ptr;
    IndicesBufferPtr ib_ptr;
    BoundingVolumnPtr bounding;
    GroupVec groups;
  };

  typedef std::shared_ptr<Mesh::SubMesh> SubMeshPtr;
  typedef std::shared_ptr<Mesh> MeshPtr;

  std::vector<SubMeshPtr>& Subs() { return subs_;}
  const std::vector<SubMeshPtr>& Subs() const { return subs_;}

  void Add(SubMeshPtr ptr) { subs_.push_back(ptr);}

  virtual void PrepareEffect(Renderer* renderer, Effect* effect) OVERRIDE {
    effect_ = effect;
  }
  virtual void Render(Renderer* renderer) OVERRIDE;

  virtual void RenderBoundingVolumn(Renderer* renderer, const Matrix4& wvp,
                                    const Camera& camera);

  bool IsVisible(const Frustrum& frustrum, const Matrix4& world);
 protected:
  void RenderSubMesh(Renderer* renderer, SubMesh* sub);
  void RenderGroup(Renderer* renderer, SubMesh* sub, const Group& group);

  Effect* effect_;
  std::vector<SubMeshPtr> subs_;
  DISALLOW_COPY_AND_ASSIGN(Mesh);
};

typedef std::shared_ptr<Mesh::SubMesh> SubMeshPtr;
typedef std::shared_ptr<Mesh> MeshPtr;

/**
 * 计算包围体
 */
SubMeshPtr ComputeBoundSphere(Mesh* mesh);
SubMeshPtr ComputeBoundAABB(Mesh* mesh);
SubMeshPtr ComputeBoundOBB(Mesh* mesh);
}  // namespace azer
