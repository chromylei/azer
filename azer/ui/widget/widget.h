#pragma once

#include <memory>
#include "base/basictypes.h"
#include "azer/base/tree_node.h"
#include "azer/base/render_export.h"
#include "ui/gfx/geometry/rect.h"

namespace azer {
namespace ui {

class Canvas;
class Context;
class AZER_EXPORT Widget : public TreeNode<Widget> {
 public:
  Widget(const gfx::Rect& rect, Context* context);
  virtual ~Widget() {}

  virtual void Hide();
  virtual void Show();
  virtual void Redraw(bool force) = 0;

  bool IsVisible() const { return visible_; }

  const gfx::Rect& GetBounds() const { return rect_;}
  const gfx::Point& origin() const { return GetBounds().origin();}

  //
  // transform point from top window position to local window position
  //
  gfx::Point ConvertPointToSurface(const gfx::Point& pt);
  gfx::Rect ConvertRectToSurface(const gfx::Rect& rc);

  Canvas* GetCanvas() { return canvas_;}
  Context* GetContext() { return context_;}
  int64 id() const { return id_;}
 protected:
  Widget(Context* context);  // root

  int64 id_;
  gfx::Rect rect_;
  bool visible_;
  bool surface_dirty_;
  Context* context_;
  Canvas* canvas_;

  friend class RootWindow;
 private:
  DISALLOW_COPY_AND_ASSIGN(Widget);
};

typedef std::shared_ptr<Widget> WidgetPtr;
}  // namespace ui
}  // namespace azer
