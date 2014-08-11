#pragma once

#include <memory>

#include "base/basictypes.h"
#include "azer/ui/widget/export.h"
#include "azer/ui/widget/widget.h"
#include "azer/render/overlay.h"
#include "azer/render/blending.h"

namespace gfx {
class Canvas;
}

namespace azer {

class Texture;
class RenderSystem;
class RootWindow;

/**
 * Window 代表一个完整的窗口
 */
class AZER_WIDGET_EXPORT Window : public azer::Widget {
 public:
  virtual ~Window();

  /**
   * rect 的坐标系是以渲染窗口左上角顶点为起点的坐标系
   */
  static Window* Create(const gfx::Rect& rect, RootWindow* root);

  virtual bool HasOverlay() { return true;}
  virtual void Render();
  virtual void Redraw(gfx::Canvas* canvas);

  /**
   * 将坐标由窗口坐标系装转换到渲染坐标系
   */
  gfx::RectF WindowToRenderer(const gfx::Rect& rc);
 private:
  Window(const gfx::Rect& rect, RootWindow* root);
  bool Init();
  void RenderWindow();
  void UpdateTexture(RenderSystem* rs);
  
  RootWindow* root_;

  OverlayEffectPtr overlay_effect_;
  BlendingPtr blending_;
  OverlayPtr overlay_;
  TexturePtr texture_;
  gfx::Canvas* canvas_;

  DISALLOW_COPY_AND_ASSIGN(Window);
};
}  // namespace azer
