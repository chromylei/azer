#pragma once

#include <deque>

#include "azer/render/render_export.h"
#include "azer/render/mesh.h"

namespace azer {

/**
 * render queue 是按照 VertexDesc 排序的一组 Renderable 队列
 * RenderQueue 一般在线下生成对应的操位
 * 即： 有那些可能存在的 VertexDesc 类型，同时还会生成一部分通用的 Shader
 * 包括镜面效果， shader map 等第一阶段运算的 shader
 * defer shading 的部分运算也可以以类似的方法完成
 * 真正渲染是 RendererObject 再身带有描述自己的 shader
 */
class AZER_EXPORT RenderQueue {
 public:
  void PreRender(RenderSystem* render_system, EffectGroup* group);
  void Render(RenderSystem* render_system);
 private:
  std::deque<RendererObject> queue_;
  DISALLOW_COPY_AND_ASSIGN(RenderQueue);
};
}  // namespace azer
