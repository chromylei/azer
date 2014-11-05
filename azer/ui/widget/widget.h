#pragma once

#include <memory>
#include "base/basictypes.h"
#include "azer/base/tree_node.h"
#include "azer/ui/widget/export.h"
#include "azer/ui/widget/widget_property.h"
#include "azer/ui/widget/theme/style_group.h"
#include "ui/gfx/geometry/rect.h"

namespace azer {
namespace ui {

class Canvas;
class Context;
class AZER_WIDGET_EXPORT Widget : public TreeNode<Widget>
                                , public StyleGroupItem {
 public:
  Widget(const gfx::Rect& rect, Context* context);
  virtual ~Widget() {}
  virtual void Redraw(bool force) = 0;

  void SetBounds(const gfx::Rect& rect);
  const gfx::Rect& bounds() const { return rect_;}
  const gfx::Point& origin() const { return bounds().origin();}
  int x() const { return bounds().x(); }
  int y() const { return bounds().y(); }
  int width() const { return bounds().width(); }
  int height() const { return bounds().height(); }
  const gfx::Size& size() const { return bounds().size(); }

  gfx::Rect GetVisibleBounds() const;

  //
  // transform point from top window position to local window position
  //
  gfx::Point ConvertPointToSurface(const gfx::Point& pt);
  gfx::Rect ConvertRectToSurface(const gfx::Rect& rc);

  Canvas* GetCanvas() { return canvas_;}
  Context* GetContext() { return context_;}
  int64 id() const { return id_;}

  void SetVisible(bool visible);
  bool visible() const { return visible_;}
  
  virtual void OnBoundsChanging(const gfx::Rect& newrect) = 0;
  virtual void OnBoundsChanged(const gfx::Rect& newrect) = 0;
  virtual void OnStyleChangled(StyleGroup* group) {}
 protected:
  Widget(Context* context);  // root

  int64 id_;
  gfx::Rect rect_;
  bool visible_;
  bool surface_dirty_;
  Context* context_;
  Canvas* canvas_;

  WidgetProperty property_;
  friend class RootWindow;
 private:
  DISALLOW_COPY_AND_ASSIGN(Widget);
};

typedef std::shared_ptr<Widget> WidgetPtr;
}  // namespace ui
}  // namespace azer
