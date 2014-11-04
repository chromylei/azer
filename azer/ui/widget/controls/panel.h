#pragma once

#include <memory>
#include "base/basictypes.h"
#include "ui/gfx/geometry/rect.h"
#include "azer/ui/widget/controls/control.h"

namespace azer {
namespace ui {

class Context;

class AZER_WIDGET_EXPORT PanelStyle {
 public:
  PanelStyle()
      : bgcolor_(0)
      , frame_color_(0)
      , round_rect_(false)
      , round_rect_radius_(0) {
  }

  uint32 bgcolor() const { return bgcolor_;}
  uint32 frame_color() const { return frame_color_;}
  bool round_rect() const { return round_rect_;}
  int32 round_rect_radius() const { return round_rect_radius_;}

  void set_bgcolor(uint32 c) { bgcolor_ = c;}
  void set_frame_color(uint32 c) { frame_color_ = c;}
  void set_round_rect(bool b) { round_rect_ = b;}
  void set_round_rect_radius(int32 v) { round_rect_radius_ = v;}
 private:
  uint32 bgcolor_;
  uint32 frame_color_;
  bool round_rect_;
  uint32 round_rect_radius_;
  DISALLOW_COPY_AND_ASSIGN(PanelStyle);
};

typedef std::shared_ptr<PanelStyle> PanelStylePtr;

class AZER_WIDGET_EXPORT Panel : public Control {
 public:
  Panel(PanelStylePtr& style, const gfx::Rect& rect, Widget* parent);
  virtual ~Panel() {}

  virtual void Redraw(bool force) OVERRIDE;
 protected:
  PanelStylePtr style_;
 private:
  DISALLOW_COPY_AND_ASSIGN(Panel);
};

}  // namespace ui
}  // namespace azer
