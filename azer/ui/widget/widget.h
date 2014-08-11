#pragma once

#include "base/basictypes.h"
#include "azer/base/tree_node.h"
#include "azer/ui/widget/export.h"
#include "ui/gfx/geometry/rect.h"

namespace gfx {
class Canvas;
}

namespace azer {

class RenderSystem;
class Widget;

class AZER_WIDGET_EXPORT Widget : public TreeNode<Widget> {
 public:
  Widget(const gfx::Rect& rect)
      : visible_(true)
      , zorder_(0)
      , rect_(rect) {
  }

  virtual ~Widget();

  virtual void Render() = 0;
  virtual void Redraw(gfx::Canvas* canvas) = 0;
  virtual bool HasSurface() { return false;}

  bool IsVisible() const { return visible_;}
  void SetVisibility(bool visible) { visible_ = visible;}

  //
  // transform point from top window position to local window position
  //
  gfx::Point ConvertPointToSurface(const gfx::Point& pt);
  gfx::Rect ConvertRectToSurface(const gfx::Rect& rc);

  /*
  gfx::Point ConvertPointToHost(const gfx::Point& pt);
  gfx::Rect ConvertRectToHost(const gfx::Rect& rc);

  gfx::Point ConvertPointFromHost(const gfx::Point& pt);
  gfx::Rect ConvertRectFromHost(const gfx::Rect& pt);
  */

  gfx::Point origin() const { return GetBounds().origin();}
  const gfx::Rect& GetBounds() const { return rect_;}

  void Sort();
 protected:
  bool visible_;
  int32 zorder_;

  gfx::Rect rect_;
  DISALLOW_COPY_AND_ASSIGN(Widget);
};
}  // namespace azer
