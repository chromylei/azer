#include "azer/ui/widget/window.h"

#include "azer/ui/widget/coordinate_transform.h"
#include "azer/ui/widget/root_window.h"
#include "ui/gfx/canvas.h"
#include "azer/render/render.h"
#include "azer/render/overlay.h"
#include "base/logging.h"

namespace azer {
Window* Window::Create(const gfx::Rect& rect, RootWindow* root) {
  std::unique_ptr<Window> win(new Window(rect, root));
  if (win->Init()) {
    return win.release();
  } else {
    return NULL;
  }
}

Window::Window(const gfx::Rect& rc, RootWindow* rt)
    : Widget(rc)
    , root_(rt)
    , canvas_(NULL) {
  root_->AddChild(this);
}

Window::~Window() {
  if (canvas_) delete canvas_;
}

bool Window::Init() {
  /**
   * 需要特别注意到是 窗口坐标系和3D坐标系在显示屏面上是不同的
   * 必须经过转换
   *
   */
  // TODO
  DCHECK(canvas_ == NULL);
  canvas_ = new gfx::Canvas(rect_.size(), 1.0f, false);

  RenderSystem* rs = root_->GetRenderSystem();

  azer::Texture::Options opt;
  opt.width = rect_.width();
  opt.height = rect_.height();
  opt.format = azer::kRGBAn8;
  opt.cpu_access = azer::kCPUWrite;
  opt.usage = azer::GraphicBuffer::kDynamic;
  opt.target = azer::Texture::kShaderResource;
  texture_.reset(rs->CreateTexture(opt));

  gfx::RectF rcf = WindowToRenderer(rect_);
  overlay_.reset(rs->CreateOverlay(rcf));
  DCHECK(overlay_.get() != NULL);
  overlay_effect_.reset(overlay_->CreateDefaultEffect());
  overlay_->SetEffect(overlay_effect_);
  overlay_effect_->SetTexture(texture_);

  Blending::Desc blending_desc;
  blending_desc.src = Blending::kSrcColor;
  blending_desc.dest = Blending::kFactor;
  blending_desc.oper = Blending::kAdd;
  blending_desc.src_alpha = Blending::kOne;
  blending_desc.dest_alpha = Blending::kZero;
  blending_desc.alpha_oper = Blending::kAdd;
  blending_desc.mask = Blending::kWriteAll;
  blending_.reset(rs->CreateBlending(blending_desc));

  Redraw(canvas_);
  
  return true;
}

gfx::RectF Window::WindowToRenderer(const gfx::Rect& rc) {
  return std::move(azer::WindowToRenderer(rc, root_->GetBounds()));
}

void Window::UpdateTexture(azer::RenderSystem* rs) {
  /*
   * Skia 采用的是 ARGB 的存储方式，并且使用 uint32 格式存储
   * D3D11 使用RGBA 且采uint8[] 的形式存储
   * 在转化过程中需要考虑内存布局对数据的影响
   */
  DCHECK(canvas_ != NULL);
  canvas_->Save();
  
  Texture::MapData mdata = std::move(texture_->map(kWriteDiscard));
  SkCanvas* canvas = canvas_->sk_canvas();
  SkBitmap bitmap;
  bitmap.setConfig(SkBitmap::kARGB_8888_Config, rect_.width(), rect_.height());
  CHECK(canvas->readPixels(&bitmap, 0, 0));
  const int kRowSize = rect_.width() * sizeof(uint32);
  for (int i = 0; i < rect_.height(); ++i) {
    uint8* dest = mdata.pdata + i * mdata.row_pitch;
    for (int j = 0; j < rect_.width() ; ++j) {
      SkColor color = bitmap.getColor(j, i);
      *dest++ = SkColorGetR(color);
      *dest++ = SkColorGetG(color);
      *dest++ = SkColorGetB(color);
      *dest++ = SkColorGetA(color);
    }
  }
  
  texture_->unmap();
}

void Window::RenderWindow() {
  RenderSystem* rs = root_->GetRenderSystem();
  DCHECK(NULL != overlay_ && canvas_ != NULL);
  UpdateTexture(rs);
}

void Window::Redraw(gfx::Canvas* canvas) {
  canvas_->DrawColor(SkColorSetARGB(0, 0, 0, 0), SkXfermode::kClear_Mode);
  Sort();

  Widget* widget = last_child();
  while (widget) {
    widget->Redraw(canvas_);
    widget = widget->prev_sibling();
  }
  RenderWindow();
}

void Window::Render() {
  DCHECK(NULL != root_);
  DCHECK(NULL != overlay_.get());
  DCHECK(NULL != overlay_effect_.get());
  RenderSystem* rs = root_->GetRenderSystem();
  float blend_factor[] = {0.75f, 0.75f, 0.75f, 1.0f};
  Renderer* renderer = rs->GetDefaultRenderer();
  renderer->UseBlending(blending_.get(), blend_factor);
  overlay_->Render(renderer);
  renderer->ResetBlending();
}
}  // namespace azer
