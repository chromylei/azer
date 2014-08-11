#pragma once

#include <vector>
#include <memory>


#include "azer/base/render_export.h"
#include "azer/render/gpu_constants_table.h"
#include "azer/render/material.h"
#include "azer/render/renderer.h"
#include "azer/render/technique.h"

namespace azer {

class Renderer;
class RenderSystem;

/**
 * class Effect
 * Effect 一组经过链接的 shader 程序和这些程序需要的常量列表
 * Effect 作为一个虚基类基本上不做任何工作，此类的子类大部分是通过
 * afxcompile 程序生成的
 * 子类会生成如何初始化 shader 及 如何与常量表交互的代码
 */
class AZER_EXPORT Effect {
 public:
  Effect(RenderSystem* rs);
  virtual ~Effect() {}

  virtual void Use(Renderer* renderer);

  // 刷新所有的 GpuConstantTable
  void flush(Renderer* renderer);

  // mtrl 通过属性与 afx 当中的 texture 相对应
  // 在 afx 生成的 c++ 代码当中，会自动生成与 material 相关的代码
  // 与 Material 相关的内容分成两个部分： 一部分是 material 相关的常量
  // 另外一部分是与 Material 相关的 Texture, 两个部分用于设置
  // 具体参考 Material 在 afx 当中可以参考 afx/material.afx 
  virtual void SetMaterial(MaterialPtr mtrl_ptr) {
    mtrl_ptr_ = mtrl_ptr;
  }
 protected:
  virtual void UseTexture(Renderer* renderer) = 0;
  void UseConstantsTable(Renderer* renderer);
  void UseTechnique(Renderer* renderer);
  TechniquePtr technique_;
  MaterialPtr mtrl_ptr_;
  std::vector<std::unique_ptr<GpuConstantsTable> > gpu_table_;
  RenderSystem* render_system_;
  DISALLOW_COPY_AND_ASSIGN(Effect);
};

typedef std::shared_ptr<Effect> EffectPtr;

typedef int32 int2[2];
typedef int32 int3[3];
typedef int32 int4[4];

typedef uint32 uint;
typedef uint32 uint2[2];
typedef uint32 uint3[3];
typedef uint32 uint4[4];

typedef bool bool2[2];
typedef bool bool3[3];
typedef bool bool4[4];

inline void Effect::UseTechnique(Renderer* renderer) {
  DCHECK(technique_.get() != NULL);
  technique_->Use(renderer);
}

inline void Effect::UseConstantsTable(Renderer* renderer) {
  DCHECK(render_system_ != NULL);
  for (int i = (int)kVertexStage; i <= (int)kPixelStage; ++i) {
    GpuConstantsTable* table = gpu_table_[i].get();
    RenderPipelineStage stage = (RenderPipelineStage)i;
    if (table != NULL) {
      table->flush(renderer);
      renderer->UseConstantsTable(stage, table);
    }
  }
}
}  // namespace
